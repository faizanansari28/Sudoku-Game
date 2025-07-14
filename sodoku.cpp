#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
using namespace sf;

const int SIZE = 9;

int board[SIZE][SIZE];
int solution[SIZE][SIZE];
bool isfixed[SIZE][SIZE];

int userstatus[9][9] = { 0 }; // 0: empty, 1: correct, 2: wrong this will store a 1 if a correct entered and 0 if empty and 2 when wrong so that 
// in the end where we compare the color we use it to color the text in the cells

/**************************************************/

bool isSafe(int row, int col, int num) {
	for (int x = 0; x < SIZE; x++)
		if (board[row][x] == num || board[x][col] == num)
			return false;

	int startRow = row - row % 3;
	int startCol = col - col % 3;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (board[startRow + i][startCol + j] == num)
				return false;

	return true;
}

bool fillBoard(int row = 0, int col = 0) {
	if (row == SIZE) return true;
	if (col == SIZE) return fillBoard(row + 1, 0);

	int numList[SIZE];
	for (int i = 0; i < SIZE; i++) numList[i] = i + 1;
	for (int i = SIZE - 1; i > 0; i--) swap(numList[i], numList[rand() % (i + 1)]);

	for (int i = 0; i < SIZE; i++) {
		if (isSafe(row, col, numList[i])) {
			board[row][col] = numList[i];
			if (fillBoard(row, col + 1)) return true;
			board[row][col] = 0;
		}
	}
	return false;
}

void storeSolution() {
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			solution[i][j] = board[i][j];
}

void makePuzzle(int missingCount = 40) {
	int count = 0;
	while (count < missingCount) {
		int row = rand() % SIZE;
		int col = rand() % SIZE;
		if (board[row][col] != 0) {
			board[row][col] = 0;
			count++;
		}
	}
}

void setFixedNumbers() {
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			isfixed[i][j] = (board[i][j] != 0);
}

bool isGameCompleted() {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] == 0 || board[i][j] != solution[i][j]) {
				return false; // either empty or wrong
			}
		}
	}
	return true; // all correct
}


/*****************************************************/


void saveGameToFile(bool fixed[9][9], int board[9][9], int solution[9][9], const char* filename) {
	std::ofstream file(filename);
	if (!file) {
		std::cout << "Error opening file for writing.\n";
		return;
	}

	// Save fixed array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file << fixed[i][j] << " ";
	file << "\n";

	// Save board array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file << board[i][j] << " ";
	file << "\n";

	// Save solution array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file << solution[i][j] << " ";
	file << "\n";

	file.close();
	std::cout << "Game saved successfully.\n";
}

void loadGameFromFile(bool fixed[9][9], int board[9][9], int solution[9][9], const char* filename) {
	std::ifstream file(filename);
	if (!file) {
		std::cout << "Error opening file for reading.\n";
		return;
	}

	// Load fixed array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file >> fixed[i][j];

	// Load board array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file >> board[i][j];

	// Load solution array
	for (int i = 0; i < 9; ++i)
		for (int j = 0; j < 9; ++j)
			file >> solution[i][j];

	file.close();
	std::cout << "Game loaded successfully.\n";
}


/***************************************************/

/****************           ***            ***********************/
/*this is the window that will ask you the name */

string getPlayerName() {
	RenderWindow window(VideoMode(1920, 1080), " Sodoku ");
	window.setFramerateLimit(60);
	RectangleShape rect(Vector2f(300,50));
	Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("first_bg.png")) {
		cout << "Error loading background image\n";
		return "Unknown";
	}
	Sprite backgroundSprite(backgroundTexture);

	Font font;
	if (!font.loadFromFile("arial.ttf")) {
		return "Unknown"; // fallback if font fails
	}

	string playerName = "";
	Text prompt(" Who is playing : ", font, 40);
	prompt.setPosition(800, 500);
	prompt.setFillColor(Color::Black);

	Text input("", font, 30);
	input.setPosition(900, 580);
	input.setFillColor(Color::Black);

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
			{
				window.close();
			}

			else if (event.type == Event::TextEntered) {
				if (event.text.unicode == '\b') {
					if (!playerName.empty()) playerName.pop_back();
				}
				else if (event.text.unicode == '\r') {
					if (!playerName.empty()) window.close(); // Press Enter
				}
				else if (event.text.unicode < 128 && playerName.length() < 20) {
					playerName += static_cast<char>(event.text.unicode);
				}
				input.setString(playerName);
			}
			
		}

		window.clear(Color::White);
		rect.setOutlineColor(Color::Black);
		rect.setOutlineThickness(2.0f);
		rect.setPosition(Vector2f(817, 570));
		/*rect.setFillColor(Color::Yellow);*/
		
		window.draw(backgroundSprite);
		window.draw(prompt);
		window.draw(rect);
		window.draw(input);
		window.display();
	}

	return playerName;
}


/****************           ***            ***********************/
void showGameOverScreen(string playerName, int secondsPlayed) {
	RenderWindow window(VideoMode(1920, 1080), "Game Over");
	window.setFramerateLimit(60);
	// Load background
	Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("background1.png")) {
		cout << "Error loading background image\n";
		exit(1);
	}
	Sprite backgroundSprite(backgroundTexture);

	// Load font
	Font font;
	if (!font.loadFromFile("arial.ttf")) {
		return;
	}

	// Convert time to string
	string minutes = to_string(secondsPlayed / 60);
	string seconds = to_string(secondsPlayed % 60);
	string timeStr = minutes + " minutes " + seconds + " seconds";

	// Save to scores.txt
	ofstream scoreOut("scores.txt", ios::app);
	scoreOut << playerName << " - " << timeStr << " (" << secondsPlayed << " seconds)\n";
	scoreOut.close();

	// Check high score from highscore.txt
	string highscoreName = "None";
	int highscoreSeconds = 1000000; // very high initial value

	ifstream highIn("highscore.txt");
	if (highIn.is_open()) {
		highIn >> highscoreName >> highscoreSeconds;
		highIn.close();
	}

	// Update high score if current is better
	if (secondsPlayed < highscoreSeconds) {
		ofstream highOut("highscore.txt");
		highOut << playerName << " " << secondsPlayed;
		highOut.close();
		highscoreName = playerName;
		highscoreSeconds = secondsPlayed;
	}

	// Convert high score to readable string
	string hsMin = to_string(highscoreSeconds / 60);
	string hsSec = to_string(highscoreSeconds % 60);
	string highscoreStr = "High Score: " + highscoreName + " - " + hsMin + " min " + hsSec + " sec";

	// Text objects
	Text gameOverText("Game Over", font, 50);
	gameOverText.setPosition(775, 235);
	gameOverText.setFillColor(Color::Red);

	Text nameText("Player: " + playerName, font, 30);
	nameText.setPosition(680, 300);
	nameText.setFillColor(Color::Black);

	Text timeText("Time: " + timeStr, font, 30);
	timeText.setPosition(680, 350);
	timeText.setFillColor(Color::Black);

	Text highText(highscoreStr, font, 30);
	highText.setPosition(680, 500);
	highText.setFillColor(Color::Green);

	// Display loop
	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear(Color::White);
		window.draw(backgroundSprite);
		window.draw(gameOverText);
		window.draw(nameText);
		window.draw(timeText);
		window.draw(highText);
		window.display();
	}
}

/*******************         ********           ******************/



void loadgamewindow()
{

	// Create a window
	RenderWindow window(VideoMode(1920, 1080), "SFML Starter");

	Texture backgroundTexture;
	backgroundTexture.loadFromFile("Background1.png");
	Sprite backgroundSprite(backgroundTexture);

	Font font;
	font.loadFromFile("arial.ttf");
	
	
	
	RectangleShape rect(Vector2f(630, 630));
	rect.setFillColor(Color::White);
	rect.setOutlineColor(Color::Black);
	rect.setOutlineThickness(3.0f);
	rect.setPosition(Vector2f(641, 228));
	

	Text Savetext("     Do You want to \n     Load the Game:", font, 50);
	Savetext.setFillColor(Color::Black);
	Savetext.setPosition(646, 335);
	

	RectangleShape Yes_rect(Vector2f(150, 50));
	Yes_rect.setFillColor(Color::Red);
	Yes_rect.setOutlineColor(Color::Black);
	Yes_rect.setOutlineThickness(3.0f);
	Yes_rect.setPosition(Vector2f(661, 698));
	

	Text Notext("No", font, 40);
	Notext.setFillColor(Color::Black);
	Notext.setPosition(686, 699);
	

	RectangleShape No_rect(Vector2f(150, 50));
	No_rect.setFillColor(Color::Red);
	No_rect.setOutlineColor(Color::Black);
	No_rect.setOutlineThickness(3.0f);
	No_rect.setPosition(Vector2f(1101, 698));
	

	Text Yestext("Yes", font, 40);
	Yestext.setFillColor(Color::Black);
	Yestext.setPosition(1106, 699);
	
	window.setFramerateLimit(60);

	// Main game loop
	while (window.isOpen())
	{
		// Event handling
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				int mouseX = event.mouseButton.x; // defines a variable that will store the mouse x position if the mouse button is pressed 
				int mouseY = event.mouseButton.y;

				if (mouseX >= 1101 && mouseX < 1251 &&
					mouseY >= 698 && mouseY < 748)
				{
					loadGameFromFile(isfixed, board, solution, "SAVE");
					window.close();
				}
				else if (mouseX >= 661 && mouseX < 811 &&
					mouseY >= 698 && mouseY < 748)
				{
					window.close();
				}

			}

		}

		// Clear screen
		window.clear(Color::White);

		// Draw everything here
		window.draw(backgroundSprite);
		window.draw(rect);
		window.draw(Savetext);
		window.draw(Yes_rect);
		window.draw(Notext);
		window.draw(No_rect);
		window.draw(Yestext);
		// Display
		window.display();
	}

}

/************************        ****        *********************/

int main()
{
	
	 
	
	
	
	/*****************/
	srand(time(0));           // initialize random number generator
	fillBoard();              // generate full board
	storeSolution();          // copy full board to solution
	makePuzzle();             // remove numbers to create the puzzle
	setFixedNumbers();        // mark fixed cells

	/**************/

	

	Vertex line[40] ; // this is the array of the points that will be joined to make the grid on window 
	int cell_size = 70;
	int index = 0;

	bool save_game = false;

	int selectedcol = -1;
	int selectedrow = -1;

	//int grid[9][9] = { 0 }; // commneted out as board declared already

	RenderWindow window(VideoMode(1920, 1080), "Sudoku Game");
	// this is the backgroung image loaded by it 
	Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("background.png")) {
		cout << "Error loading background image\n";
		return 1;
	}
	Sprite backgroundSprite(backgroundTexture);


	// now this loop is for vertical line points assigning to the vertex array 
	for (int i = 0; i < 10; i++)
	{
		int x = i * cell_size;
		line[index++] = Vertex(Vector2f(641+x, 228) , Color :: Black); // Makes a vertex with the cordinates (x,0) and the colour black 
		line[index++] = Vertex(Vector2f(641+x, 228+cell_size*9), Color::Black);
	}

	// now this loop is for horizontal line points assigning to the vertex array 
	for (int i = 0; i < 10; i++)
	{
		int y = i * cell_size;
		line[index++] = Vertex(Vector2f(641, 228+y), Color::Black);
		line[index++] = Vertex(Vector2f(641+cell_size*9, 228+y), Color::Black);
	}

	Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		return 1;
	}

	Clock gameClock;  // SFML clock to track play time
	Text timerText;   // This text class will show the time in mm:ss format

	timerText.setFont(font);
	timerText.setCharacterSize(50);
	timerText.setFillColor(Color::Black);
	timerText.setPosition(100, 900); // Position it at the bottom of the screen

	

	RectangleShape rect(Vector2f(150,50));
	rect.setFillColor(Color::Red);
	rect.setOutlineColor(Color::Black);
	rect.setOutlineThickness(3.0f);
	rect.setPosition(Vector2f(1700,900));

	Text quittext("Quit", font, 40);
	quittext.setFillColor(Color::Black);
	quittext.setPosition(1730, 902);

	

	// the first window is being called that will ask you the name and will then the string returned will be stored in the sting player name
	string playerName = getPlayerName();
	loadgamewindow();
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) // when you press the cross window closes 
				window.close();
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				int mouseX = event.mouseButton.x; // defines a variable that will store the mouse x position if the mouse button is pressed 
				int mouseY = event.mouseButton.y;

				if (mouseX >= 641 && mouseX < 641 + 9 * cell_size &&
					mouseY >= 228 && mouseY < 228 + 9 * cell_size)
				{
					selectedcol = (mouseX - 641) / cell_size;
					selectedrow = (mouseY - 228) / cell_size;
				}

			}

			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
			{
				int mouseX = event.mouseButton.x; // defines a variable that will store the mouse x position if the mouse button is pressed 
				int mouseY = event.mouseButton.y;

				if (mouseX >= 1700 && mouseX < 1850   &&
					mouseY >= 900 && mouseY <  950)
				{
					save_game = true;
				}

			}

			if (save_game)
			{
				if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
				{
					int mouseX = event.mouseButton.x; // defines a variable that will store the mouse x position if the mouse button is pressed 
					int mouseY = event.mouseButton.y;

					if (mouseX >= 1101 && mouseX < 1251 &&
						mouseY >= 698 && mouseY < 748)
					{
						saveGameToFile(isfixed, board, solution, "SAVE");
						window.close();
					}
					else if (mouseX >= 661 && mouseX < 811 &&
						mouseY >= 698 && mouseY < 748)
					{
						window.close();
					}

				}
			}

			if (event.type == Event::TextEntered)
			{
				if (selectedrow != -1 && selectedcol != -1)
				{
					char c = static_cast<char>(event.text.unicode);
					if (c >= '1' && c <= '9')
					{
						if (!isfixed[selectedrow][selectedcol]) // compares it with the fixed array and check if not in that then this will run
						{
							int val = c - '0'; // convert to int so that the int is stored in the board array 
							board[selectedrow][selectedcol] = val;

							if (val == solution[selectedrow][selectedcol])
							{
								userstatus[selectedrow][selectedcol] = 1; // correct
							}
							else
							{
								userstatus[selectedrow][selectedcol] = 2; // wrong
							}
						}
					}

					else if (c == '0' || c == 8) // clears the cell on 0 or backspace 
					{
						if (!isfixed[selectedrow][selectedcol]) // check if 0 in the fixed array then it will clear as there is not (!) also 
						{
							board[selectedrow][selectedcol] = 0;
							userstatus[selectedrow][selectedcol] = 0;
						}
					}

				}
			}
		}

		// updates the window
		window.clear(Color::White);

		window.draw(backgroundSprite);  // draw tha full-screen image

		// draws on the window which thing you want 
		window.draw(line, 40, Lines); // draws the array line with 32 points and the drawing it does is it joins the two points by a line 
		if (selectedcol != -1 && selectedrow != -1 ) // this is will run if clicked by mouse only when clicked then the selectedcol & selectedrow value changes
		{
			RectangleShape highlight; // highlight is only a variable name to make a rect shape that will highlight the specific box
			highlight.setSize(Vector2f(cell_size, cell_size)); // sets the size of the rectangle we can also do in one step that is the above one 
			highlight.setPosition( 641 + selectedcol * cell_size , 228 + selectedrow * cell_size );
			highlight.setFillColor(Color(0, 0, 255, 100));
			window.draw(highlight);
		}

		for (int row = 0; row < 9; row++)
		{
			for (int col = 0; col < 9; col++)
			{
				if (board[row][col] != 0)
				{
					Text numberText;  // text is a class that will have its object named numbertext and will use the member functions through it 
					numberText.setFont(font); // setting the font to the object
					numberText.setString(to_string(board[row][col]));
					numberText.setCharacterSize(24);
					
					if (isfixed[row][col]) // this will add colors to the numbers as fixed will black
					{
						numberText.setFillColor(Color::Black); // original puzzle numbers
					}
					else if (userstatus[row][col] == 2)
					{
						numberText.setFillColor(Color::Red); // wrong user input
					}
					else
					{
						numberText.setFillColor(Color::Black); // correct user input
					}


					numberText.setPosition(641 + col * cell_size + 20, 228 + row * cell_size + 15);
					// number output positioning
					window.draw(numberText);

				}
			}
		}

		// this will update the clock every frame 
		Time elapsed = gameClock.getElapsedTime();
		int totalSeconds = elapsed.asSeconds();
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;

		string timeStr = "";

		if (minutes < 10) timeStr += "0";
		timeStr += to_string(minutes);
		timeStr += ":";

		if (seconds < 10) timeStr += "0";
		timeStr += to_string(seconds);

		timerText.setString(timeStr);
		window.draw(timerText);// draw the text that is representing the clock 
		// this will draw a save game rectangle 
		if (save_game)
		{
			RectangleShape Save_rect(Vector2f(630, 630));
			Save_rect.setFillColor(Color::White);
			Save_rect.setOutlineColor(Color::Black);
			Save_rect.setOutlineThickness(3.0f);
			Save_rect.setPosition(Vector2f(641, 228));
			window.draw(Save_rect);

			Text Savetext("     Do You want to \n     Save the Game:", font, 50);
			Savetext.setFillColor(Color::Black);
			Savetext.setPosition(646, 335);
			window.draw(Savetext);

			RectangleShape Yes_rect(Vector2f(150, 50));
			Yes_rect.setFillColor(Color::Red);
			Yes_rect.setOutlineColor(Color::Black);
			Yes_rect.setOutlineThickness(3.0f);
			Yes_rect.setPosition(Vector2f(661, 698));
			window.draw(Yes_rect);

			Text Notext("No", font, 40);
			Notext.setFillColor(Color::Black);
			Notext.setPosition(686, 699);
			window.draw(Notext);

			RectangleShape No_rect(Vector2f(150, 50));
			No_rect.setFillColor(Color::Red);
			No_rect.setOutlineColor(Color::Black);
			No_rect.setOutlineThickness(3.0f);
			No_rect.setPosition(Vector2f(1101, 698));
			window.draw(No_rect);

			Text Yestext("Yes", font, 40);
			Yestext.setFillColor(Color::Black);
			Yestext.setPosition(1106, 699);
			window.draw(Yestext);

		}
		window.draw(rect);

		window.draw(quittext);
		window.display();
		if (isGameCompleted()) {
			int totalSeconds = gameClock.getElapsedTime().asSeconds(); // this will give the elapsed time to us that the player had played 
			window.close();  // close the current game window
			showGameOverScreen(playerName, totalSeconds);
		}

	}
}