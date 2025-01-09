#include "MyGame.h"
#include <cmath>
#include <string>

const int SCREEN_WIDTH = 800; // Define screen width
const int SCREEN_HEIGHT = 600; // Define screen height

MyGame::MyGame() : AbstractGame(), gameWon(false), gameOver(false), endMessageTimer(0), box(5, 5, 30, 30), ball({ Point2(400, 50), Vector2i(5, 0), 10, false }), lives(3), numKeys(10), score(0), ballsLaunched(0) {
    TTF_Font* font = ResourceManager::loadFont("res/fonts/arial.ttf", 24);
    Mix_Chunk* sound = ResourceManager::loadSound("res/audio/peghit.mp3");
    gfx->useFont(font);
    gfx->setVerticalSync(true);
    gfx->setWindowTitle("Peggle Inspired Game");

    // Initialize game keys
    for (int i = 0; i < numKeys; i++) {
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(getRandom(0, 750), getRandom(0, 550));
        gameKeys.push_back(k);
    }

    // Initialize pegs
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<Peg> p = std::make_shared<Peg>();
        p->isHit = false;
        p->pos = Point2(getRandom(0, 750), getRandom(SCREEN_HEIGHT / 2, SCREEN_HEIGHT - 10)); // Adjust y-coordinate range
        p->radius = 10.0; 
        pegs.push_back(p);
    }
}

MyGame::~MyGame() {

}

void MyGame::handleKeyEvents() {

    // Check if the left mouse button is clicked
    if (eventSystem->isPressed(Mouse::BTN_LEFT) && !ball.isLaunched && ballsLaunched < 15) {
        // Get the mouse position
        Point2 mousePos = eventSystem->getMousePos();
        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        // Calculate the distance from the ball to the mouse position
        float dirX = mouseX - ball.pos.x;
        float dirY = mouseY - ball.pos.y;

        // Normalize the direction vector
        float length = std::sqrt(dirX * dirX + dirY * dirY);
        dirX /= length;
        dirY /= length; 

        // Set the ball's velocity towards the mouse position
        float speed = 25.0f; 
        ball.velocity.x = dirX * speed;
        ball.velocity.y = dirY * speed;

        // Launch the ball
        ball.isLaunched = true;
        ballsLaunched++;
    }

    // Check if the "Y" key is pressed to reset the game, only if the game is won
    if (eventSystem->isPressed(Key::Y) && gameWon) {
        resetGame();
    }
}

void MyGame::update() {
    if (gameOver) {
        if (endMessageTimer < endMessageDuration) {
            endMessageTimer++;
        }
        else {
            resetGame();
            gameOver = false;
            endMessageTimer = 0;
        }
        return;
    }

    // Apply gravity to the ball's vertical velocity
    if (ball.isLaunched) {
        MyEngineSystem::Ball engineBall = { static_cast<double>(ball.pos.x), static_cast<double>(ball.pos.y), static_cast<double>(ball.radius), static_cast<double>(ball.velocity.x), static_cast<double>(ball.velocity.y) };
        mySystem->ApplyGravity(engineBall, 1.0); 
        ball.velocity.x = static_cast<float>(engineBall.vx);
        ball.velocity.y = static_cast<float>(engineBall.vy);

        // Update ball position based on velocity
        ball.pos.x += ball.velocity.x;
        ball.pos.y += ball.velocity.y;

        // Check for collisions with pegs
        for (auto& peg : pegs) {
            if (!peg->isHit) {
                MyEngineSystem::Peg enginePeg = { static_cast<double>(peg->pos.x), static_cast<double>(peg->pos.y), static_cast<double>(peg->radius) };

                if (mySystem->CheckCollision(engineBall, enginePeg)) {
                    peg->isHit = true;
                    // Handle collision response
                    mySystem->HandleCollisionResponse(engineBall, enginePeg);
                    ball.velocity.x = static_cast<float>(engineBall.vx);
                    ball.velocity.y = static_cast<float>(engineBall.vy);

                    // Increase the score by 10
                    score += 10;

                    // Play sound effect for peg hit
                    sfx->playSound(ResourceManager::getSound("res/audio/peghit.mp3"));

                    // Display hit message (EASTER EGG)
                    mySystem->EasterEgg(enginePeg);
                }
            }
        }

        // Check for collisions with the sides of the screen
        if (ball.pos.x - ball.radius < 0 || ball.pos.x + ball.radius > SCREEN_WIDTH) {
            ball.velocity.x = -ball.velocity.x; // Reverse horizontal velocity
        }
        if (ball.pos.y - ball.radius < 0) {
            ball.velocity.y = -ball.velocity.y; // Reverse vertical velocity
        }

        // Reset the ball if it hits the bottom of the screen
        if (ball.pos.y + ball.radius > SCREEN_HEIGHT) {
            ball.isLaunched = false;
            ball.pos = Point2(400, 50); // Reset position
            ball.velocity = Vector2i(5, 0); // Reset velocity

            // Check if the player has no balls remaining and has not won the game
            if (ballsLaunched >= 15 && !gameWon) {
                gameOver = true;
            }
        }

        // Check if the player has won the game
        if (score >= 100) {
            gameWon = true;
        }
    }
}

void MyGame::render() {
    // Render the ball
    gfx->setDrawColor(SDL_COLOR_WHITE);
    gfx->drawCircle(Point2(ball.pos.x, ball.pos.y), ball.radius);

    // Render the pegs
    for (const auto& peg : pegs) {
        if (!peg->isHit) {
            gfx->setDrawColor(SDL_COLOR_BLUE); // Set color for pegs
            gfx->drawCircle(Point2(peg->pos.x, peg->pos.y), peg->radius);
        }
    }
}

void MyGame::renderUI() {
    // Render the score in the top left corner
    std::string scoreText = "Score: " + std::to_string(score);
    gfx->setDrawColor(SDL_COLOR_WHITE);
    gfx->drawText(scoreText, 10, 10); // Position at (10, 10)

    // Render the number of balls remaining
    int ballsRemaining = 15 - ballsLaunched;
    std::string ballsRemainingText = "Balls Remaining: " + std::to_string(ballsRemaining);
    gfx->drawText(ballsRemainingText, 10, 30); // Position below the score

    // Render "You won!" message if the score is 100 or more
    if (score >= 100) {
        std::string winText = "You won!";
        gfx->drawText(winText, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2); // Centered position

        // Render message to press "Y" to play again
        std::string playAgainText = "Press 'Y' to play again, or press ESC to quit";
        gfx->drawText(playAgainText, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30); // Centered position
    }

    // Render "You lose!" message if no balls are remaining and the game is over
    if (ballsRemaining <= 0 && gameOver) {
        std::string loseText = "You lose!";
        gfx->drawText(loseText, SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2); // Centered position

        // Render message to tell the player that the game will reset
        std::string playAgainText = "The game will reset now.";
        gfx->drawText(playAgainText, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30); // Centered position
    }
}

void MyGame::resetGame() {
    // Reset ball position and state
    ball.pos = Point2(400, 50);
    ball.velocity = Vector2i(5, 0);
    ball.isLaunched = false;

    // Reset score and lives
    score = 0;

    // Reset game keys
    for (auto& key : gameKeys) {
        key->isAlive = true;
        key->pos = Point2(getRandom(0, 750), getRandom(0, 550));
    }

    // Reset pegs
    for (auto& peg : pegs) {
        peg->isHit = false;
        peg->pos = Point2(getRandom(0, 750), getRandom(SCREEN_HEIGHT / 2, SCREEN_HEIGHT - 10));
    }

    // Reset game won state
    gameWon = false;

    // Reset balls launched
    ballsLaunched = 0;
}