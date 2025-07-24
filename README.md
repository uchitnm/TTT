# Tic-Tac-Toe (Human vs Computer)

A classic Tic-Tac-Toe game implemented in C with GTK for the graphical user interface. Play against an intelligent computer opponent that uses strategic gameplay.

## Features

- **Graphical User Interface**: Clean and intuitive GTK-based GUI
- **Smart Computer Opponent**: Computer player with strategic decision-making
- **Real-time Status Updates**: Visual feedback for game state and turn information
- **Game Reset Functionality**: Easy restart without closing the application
- **Responsive Design**: Fixed-size window optimized for gameplay

## Computer Strategy

The computer opponent follows a strategic approach:

1. **Win if possible**: Takes any winning move immediately
2. **Block opponent**: Prevents human player from winning
3. **Center preference**: Takes the center square when available
4. **Corner strategy**: Prioritizes corner positions
5. **Edge fallback**: Takes edge positions as last resort

## Requirements

- **GTK 3.0 or later**: For GUI components
- **GCC compiler**: For building the application
- **Linux/Unix environment**: GTK is primarily supported on Unix-like systems

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libgtk-3-dev build-essential
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum install gtk3-devel gcc
# or for newer versions:
sudo dnf install gtk3-devel gcc
```

**macOS (with Homebrew):**
```bash
brew install gtk+3 pkg-config
```

## Building and Running

### Compile the Game
```bash
gcc -o tictactoe tictactoe.c `pkg-config --cflags --libs gtk+-3.0`
```

### Run the Game
```bash
./tictactoe
```

## How to Play

1. **Launch the game** by running the executable
2. **Make your move** by clicking any empty square on the 3×3 grid
3. **You play as X**, and the computer plays as O
4. **Watch the status** at the bottom of the window for turn information
5. **Reset anytime** using the "Reset Game" button
6. **Win condition**: Get three of your symbols in a row (horizontal, vertical, or diagonal)

## Game Controls

- **Left Click**: Place your X on an empty square
- **Reset Button**: Start a new game
- **Window Close**: Exit the application

## Technical Details

### Code Structure

- **Game State Management**: Centralized game state with board representation
- **Event-Driven Architecture**: GTK callback system for user interactions
- **Computer Implementation**: Algorithm-based strategy with prioritized move selection
- **Memory Management**: Proper allocation and cleanup of GTK resources

### Key Components

- `GameState` structure: Manages board state, UI elements, and game flow
- `handle_human_move()`: Processes player input and validates moves
- `handle_computer_move()`: Implements computer decision-making logic
- `check_win()` / `check_draw()`: Game state evaluation functions

## Project Structure

```
TTT/
├── README.md          # Project documentation
├── tictactoe.c        # Main source code
└── tictactoe          # Compiled executable
```

## Contributing

Feel free to fork this project and submit pull requests for:

- UI improvements
- Enhanced computer algorithms
- Additional game modes
- Bug fixes and optimizations

## License

This project is open source. Feel free to use, modify, and distribute as needed.

## Troubleshooting

**Compilation Errors:**
- Ensure GTK development libraries are installed
- Verify pkg-config is available and configured
- Check GCC compiler installation

**Runtime Issues:**
- Confirm GTK runtime libraries are installed
- Try running from a terminal to see error messages
- Ensure proper file permissions on the executable

---

Enjoy playing Tic-Tac-Toe against the computer! 🎮