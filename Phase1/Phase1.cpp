#include<iostream>
#include<fstream>
using namespace std;

class OS
{
    private:
        char M[100][4];     // Physical Memory
        char IR[4];         // Instruction Register (4 bytes)
        char R[4];          // General Purpose Register (4 bytes)
        int IC;             // Instruction Counter Register (2 bytes)
        int SI;             // Interrupt
        bool C;             // Toggle (1 byte)
        char buffer[40];    // Buffer for reading input data

    public:
        void init();        // Initialize function
        void LOAD();        // Load function
        void Execute();     // Execute function
        void MOS();         // Master Mode function

        fstream infile;     // Input file stream
        fstream outfile;    // Output file stream
};

// Initialize all variables to 0
void OS::init()
{
    // Loop through each memory location and set its content to zero
    for(int i = 0; i < 100; i++)
    {
        for(int j = 0; j < 4; j++ )
        {
            M[i][j] = 0;
        }
    }
    // Set instruction register (IR), general-purpose register (R), and toggle (C) to zero
    IR[0] = {0};
    R[0] = {0};
    C = false;
}

// Master Mode function
void OS::MOS()
{
    if(SI == 1)     // Read Mode
    {
        for(int i = 0; i <= 39; i++)
            buffer[i] = '\0';   // Clear buffer
        infile.getline(buffer, 40);    // Read data from input file into buffer
        int k = 0;
        int i = IR[2] - 48;
        i = i * 10;
        for(int l = 0; l < 10; l++)
        {
            for(int j = 0; j < 4; ++j)
            {
                M[i][j] = buffer[k];    // Store data from buffer into memory
                k++;
            }
            if(k == 40)
            {
                break;
            }
            i++;
        }

        // Display memory contents
        for(int i = 0; i < 100; i++)
        {
            cout << "M[" << i << "]\t";
            for(int j = 0; j < 4; j++)
            {
                cout << M[i][j];
            }
            cout << endl;
        }
    }
    else if(SI == 2)    // Write Mode
    {
        for(int i = 0; i <= 39; i++)
            buffer[i] = '\0';   // Clear buffer

        int k = 0;
        int i = IR[2] - 48;
        i = i * 10;

        for(int l = 0; l < 10; l++)
        {
            for(int j = 0; j < 4; j++)
            {
                buffer[k] = M[i][j];    // Store data from memory into buffer
                outfile << buffer[k];   // Write buffer content to output file
                k++;
            }
            if(k == 40)
            {
                break;
            }
            i++;
        }

        // Display memory contents
        for(int i = 0; i < 100; i++)
        {
            cout << "M[" << i << "]\t";
            for(int j = 0; j < 4; j++)
            {
                cout << M[i][j];
            }
            cout << endl;
        }

        outfile << "\n";    // Write newline character to output file
    }
    else if(SI == 3)    // Terminate
    {
        outfile << "\n";    // Write two blank lines to output file
        outfile << "\n";
    }
}

// Execution function
void OS::Execute()
{
    while(true)
    {
        for(int i = 0; i < 4; i++)    // Load instruction register
        {
            IR[i] = M[IC][i];
        }
        IC++;    // Increment instruction counter

        if(IR[0] == 'G' && IR[1] == 'D')    // GD (Get Data)
        {
            SI = 1;
            MOS();
        }
        else if(IR[0] == 'P' && IR[1] == 'D')    // PD (Put Data)
        {
            SI = 2;
            MOS();
        }
        else if(IR[0] == 'H')    // Halt
        {
            SI = 3;
            MOS();
            break;
        }
        else if(IR[0] == 'L' && IR[1] == 'R')    // LR (Load Register)
        {
            int i = IR[2] - 48;
            i = i * 10 + (IR[3] - 48);

            for(int j = 0; j <= 3; j++)
                R[j] = M[i][j];    // Load data from memory into general-purpose register
        }
        else if(IR[0] == 'S' && IR[1] == 'R')    // SR (Store Register)
        {
            int i = IR[2] - 48;
            i = i * 10 + (IR[3] - 48);

            for(int j = 0; j <= 3; j++)
                M[i][j] = R[j];    // Store data from general-purpose register into memory
        }
        else if(IR[0] == 'C' && IR[1] == 'R')    // CR (Compare Registers)
        {
            int i = IR[2] - 48;
            i = i * 10 + (IR[3] - 48);
            int count = 0;

            for(int j = 0; j <= 3; j++)
            {
                if(M[i][j] == R[j])
                    count++;    // Count number of matching bytes
            }

            if(count == 4)
                C = true;    // Set toggle if all bytes match
        }
        else if(IR[0] == 'B' && IR[1] == 'T')    // BT (Branch if True)
        {
            if(C == true)    // If toggle is true
            {
                int i = IR[2] - 48;
                i = i * 10 + (IR[3] - 48);

                IC = i;    // Branch to specified instruction
            }
        }
    }
}

// Load function
void OS::LOAD()
{
    cout << "Reading Data..." << endl;
    int x = 0;
    do
    {
        for(int i = 0; i <= 39; i++)    // Clear buffer
            buffer[i] = '\0';

        infile.getline(buffer, 40);    // Read line from input file into buffer

        for(int k = 0; k <= 39; k++)
            cout << buffer[k];    // Print buffer content

        if(buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
        {
            init();    // Initialize OS
        }
        else if(buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
        {
            IC = 00;    // Set instruction counter to 00
            Execute();    // Execute user program
        }
        else if(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
        {
            x = 0;    // Reset x
            continue;    // Continue to next iteration
        }
        else
        {
            int k = 0;

            for(; x < 100; ++x)    // Loop through memory
            {
                for(int j = 0; j < 4; ++j)
                {
                    M[x][j] = buffer[k];    // Load data from buffer into memory
                    k++;
                }

                if(k == 40 || buffer[k] == ' ' || buffer[k] == '\n')
                {
                    break;
                }
            }
        }

    } while(!infile.eof());    // Continue until end of file
}

int main()
{
    OS os;

    os.infile.open("input.txt", ios::in);    // Open input file
    os.outfile.open("output.txt", ios::out);    // Open output file

    if(!os.infile)
    {
        cout << "Failure" << endl;
    }
    else
    {
        cout << "File Exist" << endl;
    }

    os.LOAD();    // Load programs into memory
    return 0;
}