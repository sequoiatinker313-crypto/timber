//
// Created by sequoiatinker313-crypto on 7/6/26.
// Last updated by sequoiatinker313-crypto on 7/11/26.
//

// Include import libraries here
#include <iostream>
#include <random>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <vector>

// Make code easier to type with "using namespace" using namespace sf;
using namespace sf;
// Make code easier to type with "using namespace" using namespace std;
using namespace std;

// Function declaration (Function prototype)
// updateBranches does not return a value and it takes an int argument called seed
void updateBranches(int seed);
// six moving branches on the three
const int NUM_BRANCHES = 6;
vector<Sprite> branches;

// Where  is the player/branch?
// Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

int main() {
    // one high-quality central random setup
    random_device rd;
    mt19937 gen(rd());

    // PURPOSE: create a video mode object VideoMode vm(1920, 1080);
    // Brackets {} tell C++ to automatically instantiate a sf::Vector2u using those numbers
    // (A) hardcode my 2020 MacBook Pro's resolution
    // use unsigned because VideoMode constructor expects unsigned integer
    constexpr unsigned WINDOW_WIDTH = 1920;
    constexpr unsigned WINDOW_HEIGHT = 1080;
    const VideoMode VM({WINDOW_WIDTH, WINDOW_HEIGHT});
    // (B) let SFML automatically the resolution of your current monitor
    // NOTE: option B returns a list of all supported fullscreen resolutions for my monitor, sorted from highest quality to lowest quality.
    // NOTE: selecting [0] ensures I always get the best native resolution my screen can handle
    // const VideoMode vm = VideoMode::getFullscreenModes()[0];

    // PURPOSE: create and open a window for the game
    // create an object called window from SFML-provided class called RenderWindow
    // use the "Timber!!!" value to give the window a name.
    // State::Fullscreen is a value defined in SFML and take care of the integer number the internal code uses to represent a full screen.
    RenderWindow window(VM, "Timber!!!", Style::Default);

    // Create a view
    View gameView(FloatRect({0.f, 0.f}, {1920.f, 1080.f}));
    // set the window to use this and SFML will automatically scale the axis coordinates
    window.setView(gameView);

    // Create a texture to hold a graphic on the GPU Texture textureBackground;
    // Load a graphic into the texture
    const Texture TEXTUREBACKGROUND("graphics/background.png");

    // Create a sprite and attach the texture to the sprite
    Sprite spriteBackground(TEXTUREBACKGROUND);

    // Set the spriteBackground to cover the screen
    // in SFML 3, the input expects a single structure called sf::Vector2f, which is why we have the brackets.
    spriteBackground.setPosition({0.f,0.f});

    // Make a tree sprite
    // Load a graphic into the tree texture variable
    const Texture TEXTURETREE("graphics/tree.png");
    // create a sprite and attach the texture to the sprite
    Sprite spriteTree(TEXTURETREE);
    // show the texturetree texture
    spriteTree.setTexture(TEXTURETREE);
    // set the position of the tree using the coordinates x = 608 and y = 0
    constexpr float TREE_HORIZONTAL_POSITION = 810;
    constexpr float TREE_VERTICAL_POSITION = 0;
    spriteTree.setPosition({TREE_HORIZONTAL_POSITION,TREE_VERTICAL_POSITION});

    // Prepare the bee
    const Texture TEXTUREBEE("graphics/bee.png");
    Sprite spriteBee(TEXTUREBEE);
    spriteBee.setTexture(TEXTUREBEE);
    constexpr float BEE_HORIZONTAL_POSITION = 0;
    constexpr float BEE_VERTICAL_POSITION = 800;
    spriteBee.setPosition({BEE_HORIZONTAL_POSITION,BEE_VERTICAL_POSITION});
    // Is the bee currently moving?
    bool beeActive = false;
    // How fast can the bee fly?
    float beeSpeed = 0.0f;
    // get the horizontal x coordinate of the bee
    float currentPosition = spriteBee.getPosition().x;
    currentPosition += 200.0f / 5000.0f ;

    // Make 3 cloud sprites from 1 texture
    // Load 1 next texture
    const Texture TEXTURECLOUD("graphics/cloud.png");
    // 3 New sprites with the same texture
    Sprite spriteCloud1(TEXTURECLOUD);
    Sprite spriteCloud2(TEXTURECLOUD);
    Sprite spriteCloud3(TEXTURECLOUD);
    spriteCloud1.setTexture(TEXTURECLOUD);
    spriteCloud2.setTexture(TEXTURECLOUD);
    spriteCloud3.setTexture(TEXTURECLOUD);
    // Position the clouds on the left of the screen at different heights
    constexpr float CLOUD_HORIZONTAL_POSITION = 0;
    constexpr float CLOUD1_VERTICAL_POSITION = 0;
    constexpr float CLOUD2_VERTICAL_POSITION = 250;
    constexpr float CLOUD3_VERTICAL_POSITION = 500;
    spriteCloud1.setPosition({CLOUD_HORIZONTAL_POSITION,CLOUD1_VERTICAL_POSITION});
    spriteCloud2.setPosition({CLOUD_HORIZONTAL_POSITION,CLOUD2_VERTICAL_POSITION});
    spriteCloud3.setPosition({CLOUD_HORIZONTAL_POSITION,CLOUD3_VERTICAL_POSITION});
    // Are the clouds currently on screen?
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;
    // How fast is each cloud?
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    // Variables to control time itself
    Clock clock;

    // Time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize(Vector2f({timeBarStartWidth, timeBarHeight}));
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition({(1920.0f / 2) - timeBarStartWidth / 2, 980.0f});
    Time gameTimeTotal;
    float timeRemaining = 30.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;


    // Track whether the game is running
    // whenever the game is run, the variable paused will be true.
    bool paused = true;

    // We need to choose a font
    // First, load your font
    Font font;
    if (!font.openFromFile("fonts/KOMIKAP_.ttf")) {
        // Handle the loading error here safely
        cerr << "Failed to load font: " << endl;
    }

    // Draw some text
    // Initialize the score variable
    int score = 0;

    // pass the font directly into the Text constructors
    Text messageText(font);
    Text scoreText(font);

    // Set the font to our message
    messageText.setFont(font);
    scoreText.setFont(font);

    // Assign the actual message
    messageText.setString("Press Enter to start!");
    scoreText.setString("Score = 0");

    // Make it huge
    messageText.setCharacterSize(75);
    scoreText.setCharacterSize(100);

    // Choose a color
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    // Position the text
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
    messageText.setPosition({1920 / 2.0f, 1080 / 2.0f});

    // Position the scoreText at the top left with a little bit of padding
    scoreText.setPosition({20.0f, 20.0f});

    // Prepare 5 branches
    Texture textureBranch;
    const Texture TEXTUREBRANCH("graphics/branch.png");

    // Set the texture for each branch sprite
    for (int i = 0; i < NUM_BRANCHES; i++) {
        branches.emplace_back(TEXTUREBRANCH);
        branches[i].setPosition({-2000.f, -2000.f});
        // Set the sprite's origin to dead center
        // We can then spin it round without changing its position
        branches[i].setOrigin({220.f, 20.f});
    }

    updateBranches(1);
    updateBranches(2);
    updateBranches(3);
    updateBranches(4);
    updateBranches(5);

    // Prepare the player
    Texture texturePlayer;
    const Texture TEXTUREPLAYER("graphics/player.png");
    Sprite spritePlayer(TEXTUREPLAYER);
    spritePlayer.setTexture(TEXTUREPLAYER);
    spritePlayer.setPosition({580.0f, 720.0f});
    // The player starts on the left
    side playerSide = side::LEFT;

    // Prepare the gravestone
    Texture textureRIP;
    const Texture TEXTURERIP("graphics/rip.png");
    Sprite spriteRIP(TEXTURERIP);
    spriteRIP.setTexture(TEXTURERIP);
    spriteRIP.setPosition({600, 860});

    // Prepare the axe
    Texture textureAxe;
    const Texture TEXTUREAXE("graphics/axe.png");
    Sprite spriteAxe(TEXTUREAXE);
    spriteAxe.setTexture(TEXTUREAXE);
    spriteAxe.setPosition({700, 830});
    // Line the axe up with the tree
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    // Prepare the flying log
    Texture textureLog;
    const Texture TEXTURELOG("graphics/log.png");
    Sprite spriteLog(TEXTURELOG);
    spriteLog.setTexture(TEXTURELOG);
    spriteLog.setPosition({810, 720});

    // Some other useful log related variables
    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    // Control the player input
    bool acceptInput = false;

    // Prepare the sound
    SoundBuffer chopBuffer;
    if (!chopBuffer.loadFromFile("sound/chop.wav")) {
        std::cout << "didn't load chop.mav";
    }
    Sound chop(chopBuffer);

    SoundBuffer deathBuffer;
    if (!deathBuffer.loadFromFile("sound/death.wav")) {
        std::cout << "didn't load death.mav";
    }
    Sound death(deathBuffer);

    // Out of time
    SoundBuffer ootBuffer;
    if (!ootBuffer.loadFromFile("sound/out_of_time.wav")) {
        std::cout << "didn't load out_of_time.mav";
    }
    Sound outOfTime(ootBuffer);

    // Main Game Loop
    while (window.isOpen())
    {
        /*
        **************************************** Handle the players input & OS events
        ****************************************
        */

        // MANDATORY FOR macOS: Poll events so the OS allows the window to pop up!
        while (const std::optional EVENT = window.pollEvent())
        {
            // If user triggers an OS close request
            if (EVENT->is<sf::Event::Closed>())
            {
                window.close();
            }

            // Check for key presses safely via the event system
            if (const auto* keyPressed = EVENT->getIf<sf::Event::KeyPressed>())
            {

                // Quit if the user presses Escape
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }

                // Start the game if the user presses Enter/Return
                // Check to see whether the Enter/Return key is currently being pressed.
                // If it is being pressed, it sets paused to false.
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    paused = false;
                    score = 0;
                    timeRemaining = 30;

                    // Make all the branches disappear
                    for (int i = 1; i < NUM_BRANCHES; i++) {
                        branchPositions[i] = side::NONE;
                    }
                    // Make sure the gravestone is hidden
                    spriteRIP.setPosition({675, 2000});
                    // Move the player into position
                    spritePlayer.setPosition({580, 720});
                    acceptInput = true;
                }

                // Wrap the player controls to
                // Make sure we accepting input
                if (acceptInput && !paused) {
                    if (Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                        // Make sure the player is on the right
                        playerSide = side::RIGHT;
                        score++;
                        // Add to the amount of time remaining
                        timeRemaining += (2 / score) + 0.15;
                        spriteAxe.setPosition({AXE_POSITION_RIGHT, spriteAxe.getPosition().y});
                        spritePlayer.setPosition({1200, 720});
                        // Update the branches
                        updateBranches(score);
                        // Set the log flying to the left
                        spriteLog.setPosition({810, 720});
                        logSpeedX = -5000;
                        logActive = true;
                        acceptInput = false;
                        // Play a chop sound
                        chop.play();
                    }
                    // Handle the left cursor key
                    if (Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                    {
                        // Make sure the player is on the left
                        playerSide = side::LEFT;

                        score++;
                        // Add to the amount of time remaining
                        timeRemaining += (2 / score) + 0.15;
                        spriteAxe.setPosition({AXE_POSITION_LEFT, spriteAxe.getPosition().y});
                        spritePlayer.setPosition({580, 720});
                        // update the branches
                        updateBranches(score);
                        // set the log flying
                        spriteLog.setPosition({810, 720});
                        logSpeedX = 5000;
                        logActive = true;
                        acceptInput = false;
                        // Play a chop sound
                        chop.play();
                    }
                }
            }

            if (const auto* keyReleased = EVENT->getIf<sf::Event::KeyReleased>()) {
                if (!paused)
                {
                    // Listen for key presses again
                    acceptInput = true;

                    // Hide the axe off-screen
                    spriteAxe.setPosition({2000.f, spriteAxe.getPosition().y});
                }
            }
        }

        // Alternative real-time keyboard polling fallback
        if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
        {
            window.close();
        }

        /*
        **************************************** Update the scene
        ****************************************
        */

        if (!paused) {
            // Measure time
            Time dt = clock.restart();

            // Subtract from the amount of time remaining
            timeRemaining -= dt.asSeconds();
            // resize up the time bar
            timeBar.setSize(Vector2f({timeBarWidthPerSecond * timeRemaining, timeBarHeight}));

            if (timeRemaining <= 0.0f) {
                // Pause the game
                paused = true;
                // Change the message shown to the player
                messageText.setString("Out of time!!");
                // Reposition the text based on its new size
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
                messageText.setPosition({1920 / 2.0f, 1080 / 2.0f});
                // Play the out of time sound
                outOfTime.play();
            }

            // Set up the bee
            // if the bee is not active
            if (!beeActive) {

                // How fast is the bee
                // seed the random number generator
                srand(static_cast<int>(time(nullptr)) * 30);

                // get a random number between 200 and 399
                beeSpeed = (rand() % 200) + 200;

                // How high is the bee
                // seed the random number generator
                srand(static_cast<int>(time(nullptr)) *30);
                // get a random number between 500 and 999 and assign the result to a new float
                float height = (rand() % 500) + 500;
                // set the position of the bee to 2000 and the x-axis and to whatever, height, equals on the y-axis
                spriteBee.setPosition({2000, height});
                //
                beeActive = true;
            }
            else
                // Move the bee
            {
                spriteBee.setPosition({(spriteBee.getPosition().x - beeSpeed * dt.asSeconds()), spriteBee.getPosition().y});
                // Has the bee reached the left-hand edge of the screen?
                if (spriteBee.getPosition().x < -100)
                {
                    // Set it up ready to be a whole new bee next frame
                    beeActive = false;
                }
            }

            // Manage the clouds
            // Cloud 1
            if (!cloud1Active)
            {

                // How fast is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                cloud1Speed = (rand() % 200);

                // How high is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                float height = (rand() % 150);
                spriteCloud1.setPosition({-200, height});
                cloud1Active = true;
            }

            else
            {
                spriteCloud1.setPosition({spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y});

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud1.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new cloud next frame
                    cloud1Active = false;
                }
            }

            // Cloud 2
            if (!cloud2Active)
            {

                // How fast is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                cloud2Speed = (rand() % 200);

                // How high is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                float height = (rand() % 300) - 150;
                spriteCloud2.setPosition({-200, height});
                cloud2Active = true;


            }
            else
            {

                spriteCloud2.setPosition({spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y});

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud2.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new cloud next frame
                    cloud2Active = false;
                }
            }
            if (!cloud3Active)
            {

                // How fast is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                cloud3Speed = rand() % 200;

                // How high is the cloud
                srand(static_cast<int>(time(nullptr)) *30);
                float height = ((rand() % 450) - 150);
                spriteCloud3.setPosition({-200, height});
                cloud3Active = true;


            }
            else {
                spriteCloud3.setPosition({spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y});

                // Has the cloud reached the right hand edge of the screen?
                if (spriteCloud3.getPosition().x > 1920)
                {
                    // Set it up ready to be a whole new cloud next frame
                    cloud3Active = false;
                }
            }
            // Update the score text
            stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            // update the branch sprites
            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;
                if (branchPositions[i] == side::LEFT) {
                    // Move the sprite to the left side
                    branches[i].setPosition({610, height});
                    branches[i].setRotation(degrees(180));
                }
                else if (branchPositions[i] == side::RIGHT) {
                    // Move the sprite to the right side
                    branches[i].setPosition({1330, height});
                    // Set the sprite rotation to normal
                    branches[i].setRotation(degrees(0));
                }
                else {
                    // Hide the branch
                    branches[i].setPosition({3000, height});
                }
            }

            // Handle a flying log
            if (logActive)
            {
                spriteLog.setPosition({spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()), spriteLog.getPosition().y + (logSpeedY * dt.asSeconds())});
                // Has the log reached the right hand edge?
                if (spriteLog.getPosition().x < -100 ||
                    spriteLog.getPosition().x > 2000)
                {
                    // Set it up ready to be a whole new log next frame
                    logActive = false;
                    spriteLog.setPosition({810, 720});
                }
            }

            // has the player been squished by a branch?
            if (branchPositions[5] == playerSide) {
                // death
                paused = true;
                acceptInput = false;

                // Draw the gravestone
                spriteRIP.setPosition({525, 760});
                // hide the player
                spritePlayer.setPosition({2000, 660});
                // Change the text of the message
                messageText.setString("SQUISHED!!");
                // Center it on the screen
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
                messageText.setPosition({1920 / 2.0f, 1080 / 2.0f});

                // Play the death sound
                death.play();
            }

        } // End if (!paused)

        /*
        **************************************** Draw the scene
        ****************************************
        */
        // 1. Clear everything from the last frame
        window.clear();

        // 2. Draw our game scene here
        window.draw(spriteBackground);
        // draw the clouds
        window.draw(spriteCloud1);
        window.draw(spriteCloud2);
        window.draw(spriteCloud3);
        // draw the branches
        for (int i = 0; i < NUM_BRANCHES; i++) {
            window.draw(branches[i]);
        }
        // draw the tree
        window.draw(spriteTree);
        // Draw the player
        window.draw(spritePlayer);
        // Draw the axe
        window.draw(spriteAxe);
        // Draw the flying log
        window.draw(spriteLog);
        // Draw the gravestone
        window.draw(spriteRIP);
        // draw the insect
        window.draw(spriteBee);
        // draw the score
        window.draw(scoreText);
        // draw the timeBar
        window.draw(timeBar);
        if (paused) {
            // draw our message
            window.draw(messageText);
        }

        // 3. Show everything we just drew
        window.display();
    }

    // IGNORE: SecTaskLoadEntitlements failed error=22 cs_flags=20, pid=7693
    // IGNORE: SecTaskCopyDebugDescription: Timber[7693]/0#-1 LF=22
    return 0;
}

// Function definition
void updateBranches(int seed)
{
    // Move all the branches down one place
    for (int j = NUM_BRANCHES-1; j > 0; j--) {
        branchPositions[j] = branchPositions[j - 1];
    }
    // Spawn a new branch at position 0
    // LEFT, RIGHT or NONE
    // Set up a modern random engine seeded with custom seed + time
    mt19937 gen(static_cast<unsigned int>(time(nullptr)) + seed * 30);

    // define a uniform distribution between 0 and 5 inclusive (5 total possibilities)
    uniform_int_distribution<int> dist(0, 4);

    //int r = (rand() % 5);
    switch (dist(gen)) {
        // if r is equal to 0, then we add a new branch on the left-hand side at the top of the tree
        case 0:
            branchPositions[0] = side::LEFT;
            break;
        // if r is equal to 1, then the branch goes on the right.
        case 1:
            branchPositions[0] = side::RIGHT;
            break;
        // no branch at all will be added at the top
        default:
            branchPositions[0] = side::NONE;
            break;
    }
}
