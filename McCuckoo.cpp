#include <iostream>
#include <cmath>
#include <iomanip>
#include <limits>
using namespace std;

const int M = 5; // table size
const int S = 3; // stash size
const char separator = ' ';
const int width = 8;

class MultiCopyCuckoo
{
public:
    // initializing all tables with 0 as it is unoccupied
    short onChipCtr1[M] = {0};
    short onChipCtr2[M] = {0};
    int offChipTable1[M] = {0};
    int offChipTable2[M] = {0};
    int Stash[S] = {0};
    short FlagTable1[M] = {0};
    short FlagTable2[M] = {0};

    int hashFunction1(int key)
    {
        return (key + 1) % M;
    }

    int hashFunction2(int key)
    {
        return (key * 7) % M;
    }

    int hashFunctionStash(int key)
    {
        return key % S;
    }

    bool Insert_in_Table(int key)
    {
        if (Lookup(key)) // if key is already present in the table
        {
            cout << "Key is already present in the table" << endl;
            return false;
        }
        int pos1 = hashFunction1(key);
        int pos2 = hashFunction2(key);
        int counter1_value = onChipCtr1[pos1];
        int counter2_value = onChipCtr2[pos2];
        if (counter1_value == 1 && counter2_value == 1)
        {
            bool insert = Insert_in_Stash(key, pos1, pos2);
            return insert;
        }
        int key1_removed, key2_removed;
        if (counter1_value == 2)
        {
            key1_removed = offChipTable1[pos1];
        }
        if (counter2_value == 2)
        {
            key2_removed = offChipTable2[pos2];
        }
        if (counter1_value == 2 && counter2_value == 2 && key1_removed == key2_removed) // if 2 keys match the same positions in both tables
        {
            offChipTable1[pos1] = key;
            onChipCtr1[pos1] = 1;
            onChipCtr2[pos2] = 1;
            return true;
        }
        if (counter1_value == 2 && counter2_value == 2) // if both positions are occupied by keys with equal redundancy
        {
            offChipTable1[pos1] = key;
            offChipTable2[pos2] = key;
            int pos_removed2 = hashFunction2(key1_removed);
            int pos_removed1 = hashFunction1(key2_removed);
            onChipCtr1[pos_removed1] = 1;
            onChipCtr2[pos_removed2] = 1;
            onChipCtr1[pos1] = 1;
            onChipCtr2[pos2] = 1;
            return true;
        }

        if (counter1_value == 1 && counter2_value == 2) // keys in position have 2 copies and 1 copy
        {
            offChipTable2[pos2] = key;
            onChipCtr2[pos2] = 1;
            int pos_removed = hashFunction1(key2_removed);
            onChipCtr1[pos_removed] = 1;
            return true;
        }
        if (counter1_value == 2 && counter2_value == 1) // same as above case
        {
            offChipTable1[pos1] = key;
            onChipCtr1[pos1] = 1;
            int pos_removed = hashFunction2(key1_removed);
            onChipCtr2[pos_removed] = 1;
            return true;
        }
        if (counter1_value == 0 && counter2_value == 2) // redundancy of inserted element must be greater than  that of key already present in table
        {
            offChipTable1[pos1] = key;
            offChipTable2[pos2] = key;
            int pos_removed = hashFunction1(key2_removed);
            onChipCtr1[pos_removed] = 1;
            onChipCtr1[pos1] = 2;
            onChipCtr2[pos2] = 2;
            return true;
        }
        if (counter1_value == 2 && counter2_value == 0)
        {
            offChipTable1[pos1] = key;
            offChipTable2[pos2] = key;
            int pos_removed = hashFunction2(key1_removed);
            onChipCtr2[pos_removed] = 1;
            onChipCtr1[pos1] = 2;
            onChipCtr2[pos2] = 2;
            return true;
        }
        if (counter1_value == 1 && counter2_value == 0) // only 1 position where the element can be inserted
        {
            offChipTable2[pos2] = key;
            onChipCtr2[pos2] = 1;
            return true;
        }
        if (counter1_value == 0 && counter2_value == 1)
        {
            offChipTable1[pos1] = key;
            onChipCtr1[pos1] = 1;
            return true;
        }
        if (counter1_value == 0 && counter2_value == 0) // both positions are empty
        {
            offChipTable1[pos1] = key;
            offChipTable2[pos2] = key;
            onChipCtr1[pos1] = 2;
            onChipCtr2[pos2] = 2;
            return true;
        }
        return false;
    }

    bool Insert_in_Stash(int key, int pos1, int pos2) // called if insertion in main table fails
    {
        int counter = 0;
        int LinearProbeIndex = hashFunctionStash(key);
        while (Stash[LinearProbeIndex] != 0)
        {
            if (Stash[LinearProbeIndex] == -1)
            {
                break;
            }
            if (counter == S) // if all positions in stash are occupied
            {
                return false;
            }
            LinearProbeIndex = (LinearProbeIndex + 1) % S;
            counter++;
        }

        Stash[LinearProbeIndex] = key;

        FlagTable1[pos1]++;
        FlagTable2[pos2]++;
        return true;
    }
    bool Lookup_only_in_table(int key)
    {
        int CandidateBucket1 = hashFunction1(key);
        int CandidateBucket2 = hashFunction2(key);

        if (onChipCtr1[CandidateBucket1] == 0 || onChipCtr2[CandidateBucket2] == 0) // if on chip counters are 0 no need to look at table
        {
            return false;
        }

        else if (onChipCtr1[CandidateBucket1] >= 1 || onChipCtr2[CandidateBucket2] >= 1)
        {
            if ((offChipTable1[CandidateBucket1] == key) || (offChipTable2[CandidateBucket2] == key))
            {
                return true;
            }
        }
        return false;
    }

    int Lookup_in_Stash(int key) // called when lookup in table fails
    {
        int LinearProbeIndex = hashFunctionStash(key);
        int ctr = 0;
        while (ctr < S)
        {
            if (Stash[LinearProbeIndex] == key)
            {
                return LinearProbeIndex;
            }
            else if (Stash[LinearProbeIndex] == 0)
            {
                return -1;
            }
            else
            {
                LinearProbeIndex = (LinearProbeIndex + 1) % S;
            }
            ctr++;
        }
        return -1;
    }

    bool Lookup(int key) // main lookup function
    {
        if (Lookup_only_in_table(key))
        {
            return true;
        }
        else if (Lookup_in_Stash(key) != -1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Delete(int key)
    {
        int pos1 = hashFunction1(key);
        int pos2 = hashFunction2(key);
        if (Lookup_only_in_table(key)) // check if key is present in table
        {
            if (offChipTable1[pos1] == key)
                onChipCtr1[pos1] = 0;
            if (offChipTable2[pos2] == key)
                onChipCtr2[pos2] = 0;
            return true;
        }
        else
        {

            if (FlagTable1[pos1] == 0 || FlagTable2[pos2] == 0) // check if lookup in stash is necessary
            {
                return false;
            }
            int pos = Lookup_in_Stash(key);
            Stash[pos] = -1;
            FlagTable1[pos1]--;
            FlagTable2[pos2]--;
            return true;
        }
    }
    void printElement(int n)
    {

        cout << left << setw(width) << setfill(separator) << n;
    }
    void display()
    {
        cout << "|" << setw(width * 6.70) << setfill('-') << "-"
             << "|" << endl;
        cout << "|" << left << setw(width) << setfill(separator) << "ctr1"
             << "|" << left << setw(width) << setfill(separator) << "ctr2"
             << "|" << left << setw(width) << setfill(separator) << "flg1"
             << "|" << left << setw(width) << setfill(separator) << "tbl1"
             << "|" << left << setw(width) << setfill(separator) << "flg2"
             << "|" << left << setw(width) << setfill(separator) << "tbl2"
             << "|" << endl;
        cout << "|" << setw(width * 6.70) << setfill('-') << "-"
             << "|" << endl;

        for (int i = 0; i < M; i++)
        {
            cout << "|" << left << setw(width) << setfill(separator) << onChipCtr1[i]
                 << "|" << left << setw(width) << setfill(separator) << onChipCtr2[i]
                 << "|" << left << setw(width) << setfill(separator) << FlagTable1[i]
                 << "|" << left << setw(width) << setfill(separator) << offChipTable1[i]
                 << "|" << left << setw(width) << setfill(separator) << FlagTable2[i]
                 << "|" << left << setw(width) << setfill(separator) << offChipTable2[i] << "|" << endl;
        }
        cout << "|" << setw(width * 6.70) << setfill('-') << "-"
             << "|" << endl;
        cout << endl
             << "STASH MEMORY\n";
        for (int i = 0; i < S; i++)
        {
            cout << Stash[i] << " ";
        }
        cout << endl
             << endl;
    }
};

int main()
{
    MultiCopyCuckoo mccuckoo;
    cout << "Welcome\n";
    cout << "*******************************************\n";

    while (1)
    {
        bool ans = false;
        int key;
        int choice;
        cout << "1.Insert 2.Delete 3.Lookup 4.Display 5.Exit\n";
        cin >> choice;
        cout << endl;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Input must be an integer. Please try again.\n\n";
            continue;
        }
        switch (choice)
        {
        case 1:
            cout << "Enter the key to be inserted\n";
            cin >> key;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Input must be an integer. Please try again.\n\n";
                continue;
            }
            ans = mccuckoo.Insert_in_Table(key);
            if (ans == true)
                cout << "Insertion successful\n"
                     << endl;
            else
                cout << "Insertion failed either due to duplicate key or table being full. Rehashing may be required\n"
                     << endl;
            break;
        case 2:
            cout << "Enter the key to be deleted\n";
            cin >> key;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Input must be an integer. Please try again.\n\n";
                continue;
            }
            ans = mccuckoo.Delete(key);
            if (ans == true)
                cout << "Deletion successful\n"
                     << endl;
            else
                cout << "Key not present\n"
                     << endl;
            break;
        case 3:
            cout << "Enter the key to be looked up\n";
            cin >> key;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Error: Input must be an integer. Please try again.\n\n";
                continue;
            }
            ans = mccuckoo.Lookup(key);
            if (ans == true)
                cout << "Key found\n"
                     << endl;
            else
                cout << "Key not found\n"
                     << endl;
            break;
        case 4:
            mccuckoo.display();
            break;
        case 5:
            cout << "Thank You\n";
            exit(0);
        default:
            cout << "Invalid choice\n";
        }
    }
    return 0;
}