//#include<iostream>
//#include<fstream>
//#include<cstdlib>
//
//using namespace std;
//
//#define IN_FILE "test.txt"
//#define OUT_FILE "test.txt.lo"
//#define BF_SIZE 512
//
//enum crypto_mode_t { decrypt = -13, encrypt = 13 };
//
//void cryptRome(ifstream& in, ofstream& out, crypto_mode_t m) {
//    char bf[BF_SIZE];
//    if (!in.good() || !out.good()) {
//        cout << "something wrong with the file" << endl;
//        exit(1);
//    }
//
//    int key = (m == encrypt) ? encrypt : decrypt;
//    while (in.good()) {
//        in.read(bf, BF_SIZE);// 512 bytes
//
//        for (auto i = 0; i < BF_SIZE; i++) {
//            bf[i] += key;
//        }
//        out.write(bf, BF_SIZE);
//    }
//}
//
//int main(int argc, char** argv) {
//
//    // open a file and rot 13 just like romans do 
//    //ofstream out(OUT_FILE);
//    //ifstream in(IN_FILE);
//
//    //cryptRome(in, out, encrypt);
//    ofstream out(IN_FILE);
//    ifstream in(OUT_FILE);
//
//    cryptRome(in, out, decrypt);
//
//    in.close();
//    out.close();
//
//    return 0;
//}
