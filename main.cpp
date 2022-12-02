#include<iostream>
#include<bits/stdc++.h>
#include<fstream>

using namespace std;
typedef long long int ll;
typedef unsigned long long int ull;

//The entire code is explained in the attched pdf(report).

//checks if the number is a prime number.
bool isPrime(ull n) {
    for(int i = 2; i < n; i++) {
        if(n % i == 0) return false; //returns false if any number from 2 to n divides n
    }
    return true;
}

ull getPrimeProduct(vector<ull>primes) { //returns the product of all the primes in the array
    ull prime_product = 1;
    for(int prime: primes) {
        prime_product *= prime;
    }
    return prime_product;
}

vector<ull> generatePrimes(int k, ll L) { // to generate k prime numbers
    vector<ull>primes;
    int num1 = 2;
    int num2 = (int)pow((float)L, 0.5);

    while(primes.size() != k - 1) {  // k - 1 primes are from the start
        if(isPrime(num1)) {
            primes.push_back(num1);
        }
        num1++;
    }

    if(getPrimeProduct(primes) * num2 <= L) {
        num2 = L / 2 + 1;
    } 

    while(primes.size() != k) {  // The last prime number is decided such that product of all of them is greater than L
        if(isPrime(num2)) {
            primes.push_back(num2);
        }
        num2++;
    }

    return primes;
}

vector<ull>getX(vector<ull>primes) {  // returns a vector of numbers which are product of primes except that prime
    ull primes_product = getPrimeProduct(primes);
    vector<ull>X;
    for(int prime: primes) {
        X.push_back(primes_product / prime);
    }
    cout << endl;
    return X;
}

int gcdExtended(int a, int b, ll *x, ll *y) {  //returns gcd, x and y - the coefficients to be found (x, y) are passed as reference, xa + yb = gcd(a, b);
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
 
    ll x1, y1;
    int gcd = gcdExtended(b%a, a, &x1, &y1);

    *x = y1 - (b/a) * x1;
    *y = x1;
 
    return gcd;
}
//The getCoefficients function is called which uses the gcdExtended function which follows the extended euclidean algorithm and gives coefficients for two numbers such that c1x + c1y = 1. All the coefficients are generated using this gcdExtended function and by multiplying the previously generated coefficients with the recently generated coefficient of X.

vector<ll>getCoefficients(vector<ull>X) { //ci's are found such that the sum of product of ciXi = 1
    int k = X.size();
    vector<ll>coefficients(k); 
    int gcd = gcdExtended(X[0], X[1], &coefficients[0], &coefficients[1]);
    for(int i = 2; i < k; i++) {
        ll coefficient = 1;
        gcd = gcdExtended(gcd, X[i], &coefficient, &coefficients[i]);
        for(int j = 0; j < i; j++) {
            coefficients[j] *= coefficient;
        }
    } 
    return coefficients;
}
//generates the secondary keys by doing L modulo a prime number for each prime number in the generated prime numbers list (secondary_keys[i] = L modulo prime[i]) and the locker_id, valid status, k and prime numbers are entered in the configure.txt file.
vector<ull>CONFIGURE(string locker_id, int k, ll L) {
    vector<ull>secondary_keys;
    vector<ull>primes = generatePrimes(k, L);  //The prime numbers are generated in the generatePrimes function
    for(ull num: primes) secondary_keys.push_back(L % num);
    ofstream file;
    file.open("configure.txt", ios::app);
    file << locker_id << " " << "VALID" << " " << k;
    for(ull prime: primes) {
        file << " " << prime;
    }
    file << endl;
    file.close();
    return secondary_keys;
}

int USE(string locker_id, vector<ull>secondary_keys, vector<ull>primes) {
    ll L = 0;
    vector<ull>X = getX(primes);
    vector<ll>coefficients = getCoefficients(X);
    int k = primes.size();
    for(int i = 0; i < k; i++) {
        L += X[i] * coefficients[i] * secondary_keys[i];
        L = L % (ll)getPrimeProduct(primes);
    }
    //The getCoefficients function is called which uses the gcdExtended function which follows the extended euclidean algorithm and gives coefficients for two numbers such that c1x + c1y = 1. All the coefficients are generated using this gcdExtended function and by multiplying the previously generated coefficients with the recently generated coefficient of X.

    while(L < 0) {
        L = L < 0 ? L + getPrimeProduct(primes) : L;
    }
    
    
    return L;
}
//To delete an entry, the deleteEntry function is called which opens the text file, reads the entire file line wise into a string, while reading the file, when the line with locker_id is found, the position of “VALID” is found using find function and it is replaced with “INVALID” using replace function.
//The read line is now added to a string and the string is later written into the configure.txt file.

void deleteEntry(string locker_id) {
    ifstream file("configure.txt");
    string contents, line;
    string id;
    while (getline (file, line)) {
        istringstream iss(line);
        iss >> id;
        if(id == locker_id) {
            auto pos = line.find("VALID");
            line.replace(pos, 5, "INVALID");
        }
        contents += line;
        contents += "\n";
    }
    file.close();

    ofstream fileOut;
    fileOut.open("configure.txt");
    for(char c: contents) {
        fileOut << c;
    }
    fileOut.close();
    cout << "\nSuccessfully deleted";
    return;
}
//Using the get line function to read from file, each line is stored as a string and using a string stream, the locker_id and the validity status are checked and the prime numbers are returned if it exists and is a valid entry else an empty vector is returned.
vector<ull> searchEntry(string locker_id) {
    ifstream file;
    string id, line;
    int k;
    string status;
    file.open("configure.txt");
    while (getline (file, line)) {
        istringstream iss(line);
        iss >> id >> status;
        if(id == locker_id && status == "VALID") {
            iss >> k;
            vector<ull>primes(k);
            for(int i = 0; i < k; i++) {
                iss >> primes[i];
            }
            cout << endl;
            return primes;
        }
    }
    return {};
}

int main() {
    int module;
    string locker_id;
    int k;
    ll L;
    int exitStatus = 0;
    while(exitStatus != 1) {
        cout << "\nEnter 1 for CONFIGURE or 2 for USE or 3 for EXIT\n";
        cin >> module;
        int option;
        if(module == 1) {
            cout << "Enter 1 for adding a new entry, 2 for deleting an existing entry or 3 for return" << endl;
            cin >> option;
            if(option == 1) {
                cout << "\nEnter locker id: ";
                cin >> locker_id; 
                vector<ull>primes = searchEntry(locker_id);
                if(primes.size() == 0) {
                    cout << "\nEnter number of users(k): ";
                    cin >> k;
                    cout << "\nEnter the locker key L: ";
                    cin >> L;
                    vector<ull>secondary_keys = CONFIGURE(locker_id, k, L);
                    cout << "\nThe secondary keys for the users are: \n";
                    for(int i = 0; i < k; i++) {
                        cout << "u" << i + 1 << " = " << setfill('0') << setw(4) << secondary_keys[i] << endl;
                    }
                } else {
                    cout << "\nThe locker_id exists";
                }
            } else if(option == 2) {
                cout << "\nEnter the locker id to be deleted: ";
                cin >> locker_id;
                vector<ull>primes = searchEntry(locker_id);
                if( primes.size() == 0) {
                    cout << "\nInavlid entry"; 
                } else {
                    deleteEntry(locker_id);
                }
            } else if(option == 3) {
                exitStatus = 0;
            } else {
                exitStatus = 1;
            }
            
        } else if(module == 2) {
            cout << "\nEnter locker id: ";
            cin >> locker_id;
            vector<ull>primes = searchEntry(locker_id);
            if(primes.size()) {
                cout << "\nEnter number of users(k): ";
                cin >> k;
                cout << "\nEnter the secondary keys: ";
                vector<ull>secondary_keys(k);
                for(int i = 0; i < k; i++) {
                    cin >> secondary_keys[i];
                }
                L = USE(locker_id, secondary_keys, primes);
                cout << "\nThe Locker Accesscode is: " << L << endl;
            } else {
                cout << "\nThe locker id doesn't exist";
            }
        } else {
            exitStatus = 1;
        }
    }
    cout << "\nThank you!";

    return 0;
}