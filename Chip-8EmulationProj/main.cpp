#include <fstream>
#include <cstdlib>
#include <memory.h>
#include <chrono>
#include <random>
#include "Header.h"
#include <stack>

class Chip8
{
private:
	// Opcode decoding table
	std::array<void (Chip8::*)(), 0x100> table;

public:
	// Constructor
	Chip8() {
		// Initialize the opcode decoding table
		for (int i = 0; i < 0x100; i++) {
			table[i] = &Chip8::handleInvalidOpcode;
		}
		// Map opcode prefixes to their corresponding functions
		// ...
	}

	// Function to handle invalid opcodes
	void handleInvalidOpcode() {
		// Handle invalid opcode here
	}
	Chip8()
		: randGen(std::chrono::system_clock::now().time_since_epoch().count())
	{
		randByte = std::uniform_int_distribution<uint8_t>(0, 255U);


		table[0x0] = &Chip8::Table0;
		table[0x1] = &Chip8::OP_1nnn;
		table[0x2] = &Chip8::OP_2nnn;
		table[0x3] = &Chip8::OP_3xkk;
		table[0x4] = &Chip8::OP_4xkk;
		table[0x5] = &Chip8::OP_5xy0;
		table[0x6] = &Chip8::OP_6xkk;
		table[0x7] = &Chip8::OP_7xkk;
		table[0x8] = &Chip8::Table8;
		table[0x9] = &Chip8::OP_9xy0;
		table[0xA] = &Chip8::OP_Annn;
		table[0xB] = &Chip8::OP_Bnnn;
		table[0xC] = &Chip8::OP_Cxkk;
		table[0xD] = &Chip8::OP_Dxyn;
		table[0xE] = &Chip8::TableE;
		table[0xF] = &Chip8::TableF;

		for (size_t i = 0; i <= 0xE; i++)
		{
			table0[i] = &Chip8::OP_NULL;
			table8[i] = &Chip8::OP_NULL;
			tableE[i] = &Chip8::OP_NULL;
		}

		table0[0x0] = &Chip8::OP_00E0;
		table0[0xE] = &Chip8::OP_00EE;

		table8[0x0] = &Chip8::OP_8xy0;
		table8[0x1] = &Chip8::OP_8xy1;
		table8[0x2] = &Chip8::OP_8xy2;
		table8[0x3] = &Chip8::OP_8xy3;
		table8[0x4] = &Chip8::OP_8xy4;
		table8[0x5] = &Chip8::OP_8xy5;
		table8[0x6] = &Chip8::OP_8xy6;
		table8[0x7] = &Chip8::OP_8xy7;
		table8[0xE] = &Chip8::OP_8xyE;

		tableE[0x1] = &Chip8::OP_ExA1;
		tableE[0xE] = &Chip8::OP_Ex9E;

		for (size_t i = 0; i <= 0x65; i++)
		{
			tableF[i] = &Chip8::OP_NULL;
		}

		tableF[0x07] = &Chip8::OP_Fx07;
		tableF[0x0A] = &Chip8::OP_Fx0A;
		tableF[0x15] = &Chip8::OP_Fx15;
		tableF[0x18] = &Chip8::OP_Fx18;
		tableF[0x1E] = &Chip8::OP_Fx1E;
		tableF[0x29] = &Chip8::OP_Fx29;
		tableF[0x33] = &Chip8::OP_Fx33;
		tableF[0x55] = &Chip8::OP_Fx55;
		tableF[0x65] = &Chip8::OP_Fx65;
	}

	void Table0()
	{
		((*this).*(table0[opcode & 0x000Fu]))();
	}

	void Table8()
	{
		((*this).*(table8[opcode & 0x000Fu]))();
	}

	void TableE()
	{
		((*this).*(tableE[opcode & 0x000Fu]))();
	}

	void TableF()
	{
		((*this).*(tableF[opcode & 0x00FFu]))();
	}

	void OP_NULL()
	{}

	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];


public:
	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[16]{};
	uint32_t video[64 * 32]{};
	uint16_t opcode;




//////////////////////////////////////////////
//											//
//	Function to Load Contents of ROM FILE	// 
//											//
//////////////////////////////////////////////

	const unsigned int START_ADDRESS = 0x200;

	void Chip8::LoadROM(char const* filename)
	{
		std::ifstream dile(filename, std::ios::binary | std::ios::ate);

		if (file.is_open())
		{
			// Get size of file and allocate a buffer to hold the contents
			std::streampos size = file.tellg();
			char* buffer = new char[size];

			// Go back to the beginning of the file and fill the buffer
			file.seekg(0, std::ios::beg);
			file.read(buffer, size);
			file.close();

			// Load the ROM contents into the Chip8's memory, starting at 0x200
			for (long i = 0; i < size; ++i)
			{
				memory[START_ADDRESS + i] = buffer[i];
			}

			// Free the buffer
			delete[] buffer;
		}
	}

//////////////////////////////////////////////
//											//
//	    Function to Initialize PC			// 
//											//
//////////////////////////////////////////////


	Chip8::Chip8()
	{
		pc = START_ADDRESS;


		// Load Fonts into Memory
		for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
		{
			memory[FONTSET_START_ADDRESS + i] = fontset[i];
		}
	}



	//////////////////////////////////////////////
	//											//
	//	Function to Load Contents of FONTS		// 
	//											//
	//////////////////////////////////////////////


	const unsigned int FONTSET_SIZE = 80;

	uint8_t fontset[FONTSET_SIZE] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};




//////////////////////////////////////////////
//											//
//	   Function to Clear the Display		// 
//											//
//////////////////////////////////////////////

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}


//////////////////////////////////////////////
//											//
//	 Function to Return from a subroutine	// 
//											//
//////////////////////////////////////////////

void Chip8::OP_00EE()
{
	--sp;
	pc = stack[sp];
}


//////////////////////////////////////////////
//											//
//	    	Jump to location nnn		    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_1nnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = address;
}

//////////////////////////////////////////////
//											//
//	    	Call Subroutine at nnn		    // 
//											//
//////////////////////////////////////////////


void Chip8::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;

	stack[sp] = pc;
	++sp;
	pc = address;
}


//////////////////////////////////////////////
//											//
//	    Skip next routine if Vx = kk	    // 
//											//
//////////////////////////////////////////////


void Chip8::OP_3xkk()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}

//////////////////////////////////////////////
//											//
//	  Skip next instruction if Vx != kk		// 
//											//
//////////////////////////////////////////////

void Chip8::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] != byte)
	{
		pc += 2;
	}
}


//////////////////////////////////////////////
//											//
//	   Skip next instruction if Vx = Vy		// 
//											//
//////////////////////////////////////////////

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		pc += 2;
	}
}

//////////////////////////////////////////////
//											//
//	        	Set Vx = kk				    // 
//											//
//////////////////////////////////////////////


void Chip8::OP_6xkk()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;

}

//////////////////////////////////////////////
//											//
//	    	Set  Vx = Vx + kk    		    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

//////////////////////////////////////////////
//											//
//	    		Set Vx to Vy			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy0()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

//////////////////////////////////////////////
//											//
//	    	Set Vx = Vx OR Vy			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

//////////////////////////////////////////////
//											//
//	    	Set Vx = Vx AND Vy			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

//////////////////////////////////////////////
//											//
//	    	Set Vx = Vx XOR Vy			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

//////////////////////////////////////////////
//											//
//	   Set Vx = Vx + Vy, set VF = carry		// 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}
	registers[Vx] = sum & 0xFFu;
}


//////////////////////////////////////////////
//											//
//	Set Vx = Vx - Vy, set VF = NOT borrow	// 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy5()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

//////////////////////////////////////////////
//											//
//	    	Set Vx = Vx SHR 1			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

//////////////////////////////////////////////
//											//
//Set Vx = Vx = Vy - Vx, Set Vf = NOT borrow// 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

//////////////////////////////////////////////
//											//
//	    	Set Vx = Vx SHL 1			    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_8xyE()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

//////////////////////////////////////////////
//											//
//	  Skip next instruction if Vx != Vy		// 
//											//
//////////////////////////////////////////////

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

//////////////////////////////////////////////
//											//
//	    		Set i = nnn					// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;
}

//////////////////////////////////////////////
//											//
//	      Jump to location nnn + V0		    // 
//											//
//////////////////////////////////////////////

void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}

//////////////////////////////////////////////
//											//
//	    Set Vx = Random byte AND kk			// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

///////////////////////////////////////////////////////////////////////////////////////////
//																						 //
//	Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision	 //		     
//																						 //
///////////////////////////////////////////////////////////////////////////////////////////

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyon screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{

				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
		}

		}
	}
}

////////////////////////////////////////////////////////////
//														  //
//	Skip next instruction if key with value Vx is pressed // 
//														  //
////////////////////////////////////////////////////////////

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

////////////////////////////////////////////////////////////////
//															  //
//	Skip next instruction if key with value Vx is not pressed // 
//															  //
////////////////////////////////////////////////////////////////

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}

}

//////////////////////////////////////////////
//											//
//	     Set Vx = delay timer value			// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;

}

////////////////////////////////////////////////////////////////
//															  //
//	Wait for a key press, store the value of the key in Vx    // 
//															  //
////////////////////////////////////////////////////////////////

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

//////////////////////////////////////////////
//											//
//	        Set delay timer = Vx			// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delayTimer = registers[Vx];
}

//////////////////////////////////////////////
//											//
//			Set sound timer = Vx			// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Fx18()
{

	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	soundTimer = registers[Vx];
}

//////////////////////////////////////////////
//											//
//			 Set I = I + Vx					// 
//											//
//////////////////////////////////////////////

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index += registers[Vx];
}

//////////////////////////////////////////////
//											//
//  Set I = location of sprite for digit Vx // 
//											//
//////////////////////////////////////////////

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t	digit = registers[Vx];

	index = FONTSET_START_ADDRESS + (5 * digit);
}

///////////////////////////////////////////////////////////////////////////
//																		 //
//	Store BCD representation of Vx in memory locations I, I+1, and I+2   // 
//																		 //
///////////////////////////////////////////////////////////////////////////

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds place
	memory[index] = value % 10;
}

//////////////////////////////////////////////////////////////////////
//																	//
//	Store registers V0 through Vx in memory starting at location I  // 
//																	//
//////////////////////////////////////////////////////////////////////

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

///////////////////////////////////////////////////////////////////////
//																	 //  
//	Read registers V0 through Vx from memory starting at location I  // 
//																	 //
///////////////////////////////////////////////////////////////////////

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}



//////////////////////////////
//							//
//	Fetch, Decode, Execute  //
//							//
//////////////////////////////

void Chip8::Cycle()
{
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}

};

class Platform
{
public:
	Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
	{
		SDL_Init(SDL_INIT_VIDEO);

		window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCERLERATED);

		texture = SDL_CreateTexture(
			renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);

	}
	~Platform()
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	bool ProcessInput(uint8_t* keys) {
		// Store pressed keys in the keys array
		if (SDL_GetKeyboardState(nullptr) != nullptr) {
			for (int i = 0; i < 0xFF; i++) {
				keys[i] = SDL_GetKeyboardState(nullptr)[i] ? 1 : 0;
			}
		}

	void Update(void const* buffer, int pitch)
	{

		SDL_UpdateTexture(texture, nullptr, buffer, pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);

	}

	bool ProcessInput(uint8_t* keys)
	{
		bool quit = false;

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			{
				quit = true;
			}break;

			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				{
					quit = true;
				}break;
				}
				case SDLK_x:
				{
					keys[0] = 1;
				}break;

				case SDLK_1:
				{
					keys[1] = 1;
				}break;

				case SDLK_2:
				{
					keys[2] = 1;
				}break;

				case SDLK_3:
				{
					keys[3] = 1;
				}break;

				case SDLK_q:
				{
					keys[4] = 1;
				}break;

				case SDLK_w:
				{
					keys[5] = 1;
				}break;

				case SDLK_e:
				{
					keys[6] = 1;
				}break;

				case SDLK_a:
				{
					keys[7] = 1;
				}break;

				case SDLK_s:
				{
					keys[8] = 1;
				}break;

				case SDLK_d:
				{
					keys[9] = 1;
				}break;

				case SDLK_z:
				{
					keys[0xA] = 1;
				}break;

				case SDLK_c:
				{
					keys[0xB] = 1;
				}break;

				case SDLK_4:
				{
					keys[0xC] = 1;
				}break;

				case SDLK_r:
				{
					keys[0xD] = 1;
				}break;

				case SDLK_f:
				{
					keys[0xE] = 1;
				}break;

				case SDLK_v:
				{
					keys[0xF] = 1;
				}break;

				}

			}break;

		}
	}
	return quit;

}

private:
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};

};