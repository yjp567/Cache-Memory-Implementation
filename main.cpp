#include <iostream>
#include <fstream>
#include <string>
#include <cctype>        //For toUpper function
#include <cstdlib>       // For rand() and srand()
#include <ctime>         // For time()
#include <bits/stdc++.h> 

using namespace std;

#define MAX_LIMIT 100000

string binaryToHex(string binary);
void upperCase(string &input);
int isHit(string tag, string arr[], int associativity);
string hexToBinary(string hex);
int binaryToDecimal(string binary);

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    
    ifstream inputFile("cache.config"); // Open the file

    int cacheSize, blockSize, associativity;
    inputFile >> cacheSize >> blockSize >> associativity;

    string replacementPolicy, writePolicy;
    inputFile >> replacementPolicy >> writePolicy;

    inputFile.close();

    upperCase(replacementPolicy);
    upperCase(writePolicy);

    ifstream File("cache.access");

    char mode[MAX_LIMIT];
    string hexAddress[MAX_LIMIT];
    int numInstructions = 0;

    char operation;
    string address;
    char colon;
    while (File >> operation >> colon >> address && numInstructions < MAX_LIMIT)
    {
        // Store R/W operation
        mode[numInstructions] = operation;
        mode[numInstructions] = toupper(mode[numInstructions]);
      
        // Remove "0x" from the address
        if (address.substr(0, 2) == "0x")
        {
            address.erase(0, 2);
        }
        hexAddress[numInstructions] = address;

        numInstructions++;
    }
    File.close();

    for (int i = 0; i < numInstructions; i++)
    {
        int numOfzeroes = 8 - hexAddress[i].length();
        while (numOfzeroes-- > 0)
        {
            hexAddress[i] = '0' + hexAddress[i];
        }
    }

    string binAddress[numInstructions];

    for (int i = 0; i < numInstructions; i++)
    {
        binAddress[i] = hexToBinary(hexAddress[i]);
    }

    int offset = log(blockSize) / log(2);

    if (associativity == 0)
    {
        associativity = cacheSize / blockSize;
    }

    int sets = cacheSize / (blockSize * associativity); // Number of lines
    int setBits = log(sets) / log(2);

    int tagBits = 32 - setBits - offset;

    string cache[sets][associativity];
    int accessTime[sets][associativity];

    for (int i = 0; i < numInstructions; i++)
    {
        string tag = binAddress[i].substr(0, tagBits);
        string set = binAddress[i].substr(tagBits, setBits);
        int setIndex = binaryToDecimal(set);

        int hit = isHit(tag, cache[setIndex], associativity);

        cout << "Address: 0x" << hexAddress[i] << ", Set: 0x" << binaryToHex(set) << ", ";

        if (hit == -1)
            cout << "Miss";
        else
            cout << "Hit";

        cout << ", Tag: 0x" << binaryToHex(tag) << endl;

        if (hit != -1 && replacementPolicy == "LRU")
        {
            for (int j = 0; j < associativity; j++)
            {
                accessTime[setIndex][j]++;
            }
            accessTime[setIndex][hit] = 0;
        }

        if (hit == -1 && (mode[i] == 'R' || (mode[i] == 'W' && writePolicy == "WB")))
        {
            if (replacementPolicy == "FIFO")
            {
                int j;
                for (j = 0; j < associativity; j++)
                {
                    if (cache[setIndex][j] == "")
                        break;
                }
                if (j != associativity)
                {
                    cout << "here" << endl;
                    cache[setIndex][j] = tag;
                }
                else
                {
                    for (j = 0; j < associativity - 1; j++)
                    {
                        cache[setIndex][j] = cache[setIndex][j + 1];
                    }
                    cache[setIndex][associativity - 1] = tag;
                }
            }
            else if (replacementPolicy == "LRU")
            {
                int k = 0;
                for (k = 0; k < associativity; k++)
                {
                    if (cache[setIndex][k] == "")
                        break;
                }
                if (k == associativity)
                {
                    int targetIndex = 0;
                    for (int j = 1; j < associativity; j++)
                    {
                        if (accessTime[setIndex][j] > accessTime[setIndex][targetIndex])
                            targetIndex = j;
                    }
                    cache[setIndex][targetIndex] = tag;
                }
                else
                {
                    cache[setIndex][k] = tag;
                }
            }
            else if (replacementPolicy == "RANDOM")
            {
                int k = 0;
                for (k = 0; k < associativity; k++)
                {
                    if (cache[setIndex][k] == "")
                        break;
                }
                if (k == associativity)
                {
                    int randomNumber = rand() % associativity;
                    cache[setIndex][randomNumber] = tag;
                }
                else
                {
                    cache[setIndex][k] = tag;
                }
            }
        }
    }

    return 0;
}

string binaryToHex(string binary)
{
    string hexString = "";

    // Modify the binary string with leading zeros to make its length a multiple of 4
    int numZero = 4 - binary.length() % 4;
    string newBinary = string(numZero, '0') + binary;

    for (int i = 0; i < newBinary.length(); i += 4)
    {
        int decimalValue = binaryToDecimal(newBinary.substr(i, 4));
        hexString += (decimalValue < 10) ? (char)('0' + decimalValue) : (char)('A' + (decimalValue - 10));
    }
    return hexString;
}

void upperCase(string &input)
{
    for (char &c : input)
    {
        c = toupper(c);
    }
}

int isHit(string tag, string arr[], int associativity)
{
    for (int i = 0; i < associativity; i++)
    {
        if (tag == arr[i])
            return i;
    }
    return -1;
}

string hexToBinary(string hex)
{
    string binary;

    for (int i = 0; i < hex.length(); i++)
    {
        if (hex[i] == '0')
        {
            binary.append("0000");
        }
        else if (hex[i] == '1')
        {
            binary.append("0001");
        }
        else if (hex[i] == '2')
        {
            binary.append("0010");
        }
        else if (hex[i] == '3')
        {
            binary.append("0011");
        }
        else if (hex[i] == '4')
        {
            binary.append("0100");
        }
        else if (hex[i] == '5')
        {
            binary.append("0101");
        }
        else if (hex[i] == '6')
        {
            binary.append("0110");
        }
        else if (hex[i] == '7')
        {
            binary.append("0111");
        }
        else if (hex[i] == '8')
        {
            binary.append("1000");
        }
        else if (hex[i] == '9')
        {
            binary.append("1001");
        }
        else if (hex[i] == 'a' || hex[i] == 'A')
        {
            binary.append("1010");
        }
        else if (hex[i] == 'b' || hex[i] == 'B')
        {
            binary.append("1011");
        }
        else if (hex[i] == 'c' || hex[i] == 'C')
        {
            binary.append("1100");
        }
        else if (hex[i] == 'd' || hex[i] == 'D')
        {
            binary.append("1101");
        }
        else if (hex[i] == 'e' || hex[i] == 'E')
        {
            binary.append("1110");
        }
        else if (hex[i] == 'f' || hex[i] == 'F')
        {
            binary.append("1111");
        }
    }

    return binary;
}

int binaryToDecimal(string binary)
{
    int index = binary.length() - 1;
    int decimal = 0;
    int factor = 1;

    while (index >= 0)
    {
        decimal += factor * (binary[index] - '0');
        factor *= 2;
        index--;
    }

    return decimal;
}
