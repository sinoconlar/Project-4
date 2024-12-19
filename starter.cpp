#include <algorithm>
#include <iostream>
#include <vector>
// ifdef is a compiler flag, to control when the compiler will include specific lines of code
// in this case, we are going to import windows headers only on windows systems
#ifdef _WIN32
#include <windows.h>
#endif

// color code constants https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
// backgrounds
const std::string BG_BLACK = "\033[40m";
const std::string BG_WHITE = "\033[100m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
// colors
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
// reset colors
const std::string CLEAR = "\033[0m\033[49m";
const std::string RESET = "\033[2J\033[H";

// ascii representation of chess pieces
const std::string ROOK = "r ";
const std::string KNIGHT = "n ";
const std::string BISHOP = "b ";
const std::string QUEEN = "q ";
const std::string KING = "k ";
const std::string PAWN = "p ";
const std::string BK_ROOK = "R ";
const std::string BK_KNIGHT = "N ";
const std::string BK_BISHOP = "B ";
const std::string BK_QUEEN = "Q ";
const std::string BK_KING = "K ";
const std::string BK_PAWN = "P ";
const std::string EMPTY = ". ";
// unicode representation of chess pieces
// add an extra space after unicode characters because windows
// renders these at 1.5 size which causes overlapping
const std::string UNI_ROOK = "♜ ";
const std::string UNI_KNIGHT = "♞ ";
const std::string UNI_BISHOP = "♝ ";
const std::string UNI_QUEEN = "♛ ";
const std::string UNI_KING = "♚ ";
const std::string UNI_PAWN = "♟ ";
const std::string UNI_BK_ROOK = "♖ ";
const std::string UNI_BK_KNIGHT = "♘ ";
const std::string UNI_BK_BISHOP = "♗ ";
const std::string UNI_BK_QUEEN = "♕ ";
const std::string UNI_BK_KING = "♔ ";
const std::string UNI_BK_PAWN = "♙ ";
const std::string UNI_EMPTY = ". ";

// Position represents a coordinate position on the chessboard
struct Position {
  int x;
  int y;
  Position(int x, int y) : x(x), y(y) {}
};

// IGamePiece represents a generic chess piece
// all chess pieces inherit from this class
class IGamePiece {
public:
  // isWhite is a boolean to track what team the piece belongs to
  bool isWhite = false;
  // position represents the piece coordinates on the game board
  // this position will be updated by main() after every board update
  Position position = Position(0, 0);
  // returns the name of the gamePiece
  virtual std::string getName() = 0;
  // returns a string representing the playing piece
  virtual std::string render() = 0;
  // returns a vector of positions where the piece could move
  virtual std::vector<Position> getPotentialMoves() = 0;
  virtual ~IGamePiece() = default;
};

// BoardManager is a globally avalble object to hold information about the boardgame state
class BoardManager {
private:
  std::vector<std::vector<IGamePiece *>> board; // 2D vector grid to represent the chessboard

public:
  // prepareBoard creates a chessboard data structure
  //  implement this *after* all gamePiece declarations so that they can be referenced
  void prepareBoard();

  // getAtPosition returns a pointer to the IGamePiece located at the specified position on the board
  IGamePiece *const getAtPosition(int row, int col) { return board[row][col]; }

  // renderBoard prints a colored grid to the terminal representing a chessboard with pieces
  // this also adds highlights for the cursor, selected pieces, and potential moves when applicable
  void renderBoard(Position cursor, IGamePiece *selectedPiece, std::vector<Position> moves) {
    int rows = board.size();
    // set `cols` (columns) is zero if there are no rows, prevents crash with empty boards.
    int cols = rows == 0 ? 0 : board[0].size();
    // iterate through all positions on the chessboard printing to the console
    for (int col = 0; col < cols; col++) {
      for (int row = 0; row < rows; row++) {
        for (auto &move : moves) { // if a position is in the potential move set, highlight positions in red
          if (move.x == row && move.y == col) {
            std::cout << BG_RED;
            break;
          }
        }
        if (selectedPiece != nullptr) { // if a piece is currently selected, highlight it in green
          if (selectedPiece->position.x == row && selectedPiece->position.y == col)
            std::cout << BG_GREEN;
        }
        bool highlighted = (row == cursor.x && col == cursor.y);
        if (highlighted) // use white background to create highlight effect to represent the cursor
          std::cout << BG_WHITE;
        if (board[row][col] == nullptr) { // print empty space characters
          std::cout << EMPTY;
        } else {
          // print icon for piece at current position
          std::cout << board[row][col]->render();
          // update piece position data to match current board position
          board[row][col]->position = Position(row, col);
        }
        std::cout << CLEAR << BG_BLACK; // reset text and background colors back to default
      }
      std::cout << '\r' << std::endl; // begin next row
    }
  }

  // movePiece moves an IGamePiece to a new position on the board
  // returns true on success
  bool movePiece(IGamePiece *piece, Position target) {
    // check that move is valid (by checking that the move is in the PotentialMoves list)
    bool isValidMove = false;
    for (auto &move : piece->getPotentialMoves()) {
      if (move.x == target.x && move.y == target.y) {
        isValidMove = true;
        break;
      }
    }
    if (!isValidMove)
      return false;
    // delete existing piece at new position if present
    if (board[target.x][target.y] != nullptr) {
      delete board[target.x][target.y];
      board[target.x][target.y] = nullptr;
    }
    // perform move
    Position oldPos = piece->position;
    board[target.x][target.y] = piece;   // update pointer at new board position to point to gamePiece
    board[oldPos.x][oldPos.y] = nullptr; // delete reference to piece at original board position
    return true;
  }

  // you may want to add more helper functionality here, such as checking if the a position is valid on the board
};

// declare global static boardManager for easy access anywhere
// so all gamePiece implemenentations can access this object
static BoardManager boardManager;

/* example
class Plusser : public IGamePiece {
public:
  std::string getName() override {
    std::string color = isWhite ? "White " : "Black ";
    return color + "Plusser";
  }
  std::string render() override {
    if (isWhite) {
      return "⚔ "; // use a different symbol for each color
    } else {
      return "+ ";
    }
  }
  virtual std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    // add potential moves
    int potentialMoves[4][2] = {
      {position.x + 1, position.y}, // right
      {position.x - 1, position.y}, // left
      {position.x, position.y + 1}, // down
      {position.x, position.y - 1}  // up
    };

    for (auto move : potentialMoves) {
      int x = move[0];
      int y = move[1];

      // Check if the move is within the bounds of the board
      if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        IGamePiece *targetPiece = boardManager.getAtPosition(x, y);
        // If there is a piece at the target position, check if it belongs to the same team
        if (targetPiece != nullptr && targetPiece->isWhite == this->isWhite) {
          continue;  // Skip the move if the target piece is the same color
        }
        // Add valid move
        moves.push_back(Position(x, y));
      }
    }
    return moves;
  }
};
*/
class King : public IGamePiece {
public:
  King(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White King" : "Black King");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? KING : BK_KING; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a King
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
    // Moves one square in any direction.
    int kingMoves[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, 
                           {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (auto move : kingMoves) {
      int x = position.x + move[0];
      int y = position.y + move[1];

      // Check if the move is within the bounds of the board
      if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        IGamePiece *targetPiece = boardManager.getAtPosition(x, y);
        // If there is a piece at the target position, check if it belongs to the same team
        if (targetPiece != nullptr && targetPiece->isWhite == this->isWhite) {
            continue;  // Skip the move if the target piece is the same color
        }
        // Add valid move
        moves.push_back(Position(x, y));
      }
    }
    return moves;
  }
};

class Knight : public IGamePiece {
public:
  Knight(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White Knight" : "Black Knight");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? KNIGHT : BK_KNIGHT; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a Knight
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
    // Define all potential knight moves in an L-shape
    int knightMoves[8][2] = {
      {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
      {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    // Check all potential moves
    for (auto move : knightMoves) {
      Position newPos(position.x + move[0], position.y + move[1]);
      
      // Ensure the new position is within bounds of the chessboard
      if (newPos.x >= 0 && newPos.x < 8 && newPos.y >= 0 && newPos.y < 8) {
        IGamePiece *pieceAtTarget = boardManager.getAtPosition(newPos.x, newPos.y);
        
        // If the target position is empty or occupied by an opponent's piece, add the move
        if (pieceAtTarget == nullptr || pieceAtTarget->isWhite != this->isWhite) {
          moves.push_back(newPos);
        }
      }
    }
    return moves;
  }
};

// The piece implements the "first-move" rule (WIP)
class Pawn : public IGamePiece {
public:
  bool hasMoved = false; // Track if the pawn has moved

  Pawn(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White Pawn" : "Black Pawn");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? PAWN : BK_PAWN; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a Pawn
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
  // Define possible move directions based on color
  int direction = isWhite ? -1 : 1;  // White moves up (y - 1), Black moves down (y + 1)
  int startRow = isWhite ? 6 : 1;  // White pawns start at row 6, Black pawns start at row 1
  
  // Move one square forward
  int forwardX = position.x;
  int forwardY = position.y + direction;
  if (forwardY >= 0 && forwardY < 8) {
    IGamePiece *targetPiece = boardManager.getAtPosition(forwardX, forwardY);
    // Only move if the target is empty
    if (targetPiece == nullptr) {
      moves.push_back(Position(forwardX, forwardY));
    }
  }

  // If the pawn hasn't moved yet, allow moving two squares forward
  if (!hasMoved) {
    int doubleForwardY = position.y + 2 * direction;
    if (doubleForwardY >= 0 && doubleForwardY < 8) {
      IGamePiece *targetPiece = boardManager.getAtPosition(forwardX, doubleForwardY);
      if (targetPiece == nullptr) {
        moves.push_back(Position(forwardX, doubleForwardY));
      }
    }
  }

  // Pawn captures diagonally
  int captureX[2] = { position.x + 1, position.x - 1 };  // Capture diagonally to the right or left
  for (int i = 0; i < 2; ++i) {
    int captureY = position.y + direction;  // Captures go in the same direction as forward movement
    if (captureX[i] >= 0 && captureX[i] < 8 && captureY >= 0 && captureY < 8) {
      IGamePiece *targetPiece = boardManager.getAtPosition(captureX[i], captureY);
      if (targetPiece != nullptr && targetPiece->isWhite != this->isWhite) {
        moves.push_back(Position(captureX[i], captureY));  // Capture if the target is an opponent's piece
      }
    }
  }
    return moves;
  }

  // After the pawn moves, update the hasMoved status
  void movePiece() {
    hasMoved = true;
  }
};

class Rook : public IGamePiece {
public:
  Rook(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White Rook" : "Black Rook");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? ROOK : BK_ROOK; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a Rook
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
    int directions[4][2] = {
      {1, 0},  // Right
      {-1, 0}, // Left
      {0, 1},  // Down
      {0, -1}  // Up
    };

    // Check all directions
    for (auto direction : directions) {
      int x = position.x;
      int y = position.y;

      // Move in the current direction until hitting the edge or a piece
      while (true) {
        x += direction[0];
        y += direction[1];

        // Check if the new position is within bounds
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
          break; // Stop if the move goes out of bounds
        }

        IGamePiece *targetPiece = boardManager.getAtPosition(x, y);

        // If the target position is occupied by an allied piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite == this->isWhite) {
          break; // Blocked by a friendly piece, stop in this direction
        }

        // If the target position is empty or occupied by an opponent's piece, add the move
        moves.push_back(Position(x, y));

        // If the target position is occupied by an opponent's piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite != this->isWhite) {
          break; // Capture an enemy piece, then stop
        }
      }
    }
    return moves;
  }
};

class Bishop : public IGamePiece {
public:
  Bishop(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White Bishop" : "Black Bishop");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? BISHOP : BK_BISHOP; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a Bishop
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
    int directions[4][2] = {
      {1, 1},   // Right-Down
      {-1, 1},  // Left-Down
      {1, -1},  // Right-Up
      {-1, -1}  // Left-Up
    };

    // Check all four diagonal directions
    for (auto direction : directions) {
      int x = position.x;
      int y = position.y;

      // Move in the current diagonal direction until hitting the edge or a piece
      while (true) {
        x += direction[0];
        y += direction[1];

        // Check if the new position is within bounds
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
          break; // Stop if the move goes out of bounds
        }

        IGamePiece *targetPiece = boardManager.getAtPosition(x, y);

        // If the target position is occupied by an allied piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite == this->isWhite) {
          break; // Blocked by a friendly piece, stop in this direction
        }

        // If the target position is empty or occupied by an opponent's piece, add the move
        moves.push_back(Position(x, y));

        // If the target position is occupied by an opponent's piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite != this->isWhite) {
          break; // Capture an enemy piece, then stop
        }
      }
    }
    return moves;
  }
};

// Combined Rook and Bishop moves
class Queen : public IGamePiece {
public:
  Queen(bool isWhite, int x, int y) {
    this->isWhite = isWhite;
    this->position = Position(x, y);
  }

  // Override getName to return the name of the piece
  std::string getName() override {
    return (isWhite ? "White Queen" : "Black Queen");
  }

  // Override render to return a representation of the piece
  std::string render() override {
    return isWhite ? QUEEN : BK_QUEEN; // Using ASCII for display
  }

  // Override getPotentialMoves to return possible moves for a Queen
  std::vector<Position> getPotentialMoves() override {
    std::vector<Position> moves;
    
    // All 8 possible movement directions (combining Rook and Bishop moves)
    int directions[8][2] = {
      {1, 0},   // Right (Rook)
      {-1, 0},  // Left (Rook)
      {0, 1},   // Down (Rook)
      {0, -1},  // Up (Rook)
      {1, 1},   // Right-Down (Bishop)
      {-1, 1},  // Left-Down (Bishop)
      {1, -1},  // Right-Up (Bishop)
      {-1, -1}  // Left-Up (Bishop)
    };

    // Iterate through all 8 directions (both Rook and Bishop)
    for (auto direction : directions) {
      int x = position.x;
      int y = position.y;

      // Move in the current direction until hitting the edge or a piece
      while (true) {
        x += direction[0];
        y += direction[1];

        // Check if the new position is within bounds
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
          break; // Stop if the move goes out of bounds
        }

        IGamePiece *targetPiece = boardManager.getAtPosition(x, y);

        // If the target position is occupied by an allied piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite == this->isWhite) {
          break; // Blocked by a friendly piece, stop in this direction
        }

        // If the target position is empty or occupied by an opponent's piece, add the move
        moves.push_back(Position(x, y));

        // If the target position is occupied by an opponent's piece, stop in this direction
        if (targetPiece != nullptr && targetPiece->isWhite != this->isWhite) {
          break; // Capture an enemy piece, then stop
        }
      }
    }
    return moves;
  }
};

// Implement prepareBoard *after* declaring all pieces so they can be referenced here and placed on the board
void BoardManager::prepareBoard() {
  // create an 8x8 chessboard defaulting to null pointers of IGamePiece objects
  board = std::vector<std::vector<IGamePiece *>>(8, std::vector<IGamePiece *>(8, nullptr));
  // [column][row]
  // Row 0 (Black pieces)
  board[0][0] = new Rook(false, 0, 0); // Black Rook
  board[1][0] = new Knight(false, 1, 0); // Black Knight
  board[2][0] = new Bishop(false, 2, 0); // Black Bishop
  board[3][0] = new Queen(false, 3, 0); // Black Queen
  board[4][0] = new King(false, 4, 0); // Black King
  board[5][0] = new Bishop(false, 5, 0); // Black Bishop
  board[6][0] = new Knight(false, 6, 0); // Black Knight
  board[7][0] = new Rook(false, 7, 0); // Black Rook

  // Row 1 (Black Pawns)
  for (int i = 0; i < 8; i++) {
    board[i][1] = new Pawn(false, i, 1);
  }

  // Row 6 (White Pawns)
  for (int i = 0; i < 8; i++) {
    board[i][6] = new Pawn(true, i, 6);
  }

  // Row 7 (White Pieces)
  board[0][7] = new Rook(true, 0, 7);
  board[1][7] = new Knight(true, 1, 7);
  board[2][7] = new Bishop(true, 2, 7);
  board[3][7] = new Queen(true, 3, 7);
  board[4][7] = new King(true, 4, 7);
  board[5][7] = new Bishop(true, 5, 7);
  board[6][7] = new Knight(true, 6, 7);
  board[7][7] = new Rook(true, 7, 7);
}

// you shouldnt need to modify main(), but you are free to change it if you want
int main() {
#ifndef _WIN32              // the next line only runs on non-windows systems
  system("stty raw -echo"); // Disable line buffering and echo on linux/macos
#else
  SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT)); // disable line buffering and echo on windows
#endif
  printf("\033[?25l");   // hide the cursor
  printf("\033[?1049h"); // use alternate screen buffer
  // populate terminal before starting...
  std::cout << BG_BLACK << RESET; // Clear screen and use dark background
  printf("Controls: Arrow Keys, Space to Select ('q' to quit)\n\r");
  boardManager.prepareBoard();      // populate chessboard
  Position cursor = Position(0, 0); // start cursor in top left corner

  IGamePiece *selectedPiece = nullptr; // reference to the currently selected gamePiece, start deselected
  std::vector<Position> moves;         // vector of potential moves for the selectedPiece

  boardManager.renderBoard(cursor, selectedPiece, moves); // print the initial board to the console

  while (true) {             // user interaction loop
    std::string status = ""; // reset status text after any interaction
    char c = std::cin.get(); // wait for keyboard input
    // ... after user enters a key.
    std::cout << BG_BLACK << RESET;                                             // Clear screen and use dark background
    printf("Controls: Arrow Keys, Space to Select ('q' to quit)\n\r"); // print help text at top
    // process user input
    if (c == '\033') { // if control characterwas pressed, we need to capture the following control characters and process them
      char seq1 = std::cin.get();
      char seq2 = std::cin.get();
      if (seq1 == '[') { // control character may be an arrow if sequence starts with `\033[`
        switch (seq2) {  // if arrow key was pressed, move cursor around the board
        case 'A':        // up arrow
          cursor.y = std::clamp(cursor.y - 1, 0, 7);
          break;
        case 'B': // down arrow
          cursor.y = std::clamp(cursor.y + 1, 0, 7);
          break;
        case 'C': // right arrow
          cursor.x = std::clamp(cursor.x + 1, 0, 7);
          break;
        case 'D': // left arrow
          cursor.x = std::clamp(cursor.x - 1, 0, 7);
          break;
        }
        if (selectedPiece != nullptr) // if a piece is currently selected, add its name to the status message
          status += selectedPiece->getName() + " selected";
      }
    } else if (c == 'q' || c == 3) { // quit on 'q' or ctrl+c
      printf("\033[?25h");           // show the cursor
      printf("\033[?1049l");         // Restore the main screen buffer
      system("stty sane");           // Restore terminal settings
      printf("Exiting...\n");
      break;
    } else if (c == ' ') {                                 // "select" (space or return key pressed)
      if (selectedPiece != nullptr) {                      // if a piece is currently selected
        if (boardManager.movePiece(selectedPiece, cursor)) // attempt move if a potential move position is selected
          status += "Moved " + selectedPiece->getName();
        else
          status += "Deselected";
        selectedPiece = nullptr; // clear the selectedPiece and the potential moves list after any selection is made
        moves = std::vector<Position>();
      } else {                                                           // if a piece is not currently selected
        if (boardManager.getAtPosition(cursor.x, cursor.y) == nullptr) { // do nothing but update status message if an empty space is selected
          status += "Empty space selected";
        } else { // set the selectedPiece reference and update the potentialmoves list for the piece
          selectedPiece = boardManager.getAtPosition(cursor.x, cursor.y);
          moves = selectedPiece->getPotentialMoves();
          status += selectedPiece->getName() + " selected";
        }
      }
    }
    boardManager.renderBoard(cursor, selectedPiece, moves); // render the board and print the status message
    std::cout << status;
  }
}