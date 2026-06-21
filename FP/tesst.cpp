#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <conio.h>

using namespace std;

struct Akun { string username, password, role, lokasi, spesialisasi; float rating; };
struct Pesanan { int id; string pelanggan, cleaner, status; int biaya, durasi; };

Akun dbAkun[10] = {
    {"pelanggan1", "123", "pelanggan", "Bekasi","", 0}, 
    {"p", "1", "pelanggan", "Bekasi","deep clean", 0}, 
    {"c", "1", "cleaner", "Bekasi","regular", 4.5},
    {"mitra2", "123", "cleaner", "Jakarta","deep clean", 3.5},
    {"mitra3", "123", "cleaner", "Bandung","carpet cleaning", 1.5},
    {"mitra4", "123", "cleaner", "Bekasi","sofa cleaning", 2.0}
};
int jmlAkun = 6;

Pesanan dbPesanan[10];
int jmlPesanan = 0, nextIdPesanan = 1;

string currentUser = ""; 
string currentRole = "";

void pauseLayar() {
    cout << "\nTekan Enter untuk lanjut...";
    cin.ignore();
    cin.get();
}

bool textFix(string teks, string keyword) {
    for (char& c : teks)    c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    for (char& c : keyword) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return teks.find(keyword) != string::npos;
}

bool usernameExists(const string& user) {
    for (int i = 0; i < jmlAkun; i++) {
        if (dbAkun[i].username == user) {
            return true;
        }
    }
    return false;
}

void clearScreen() {
    system("cls");
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(50));
}
void login() {
    clearScreen();
    string user, pass;
    cout << "\n--- FORM LOGIN ---\n";
    cout << "Username: "; cin >> user;
    cout << "Password: "; cin >> pass;
    
    for (int i = 0; i < jmlAkun; i++) {
        if (dbAkun[i].username == user && dbAkun[i].password == pass) {
            currentUser = user; 
            currentRole = dbAkun[i].role;
            cout << "Login Sukses! Selamat datang, " << currentUser << " (" << currentRole << ")\n"; 
            pauseLayar();
            clearScreen();
            return;
        }
    }
    cout << "Login Gagal! Username atau password salah.\n";
    pauseLayar();
    clearScreen();
}

void registerAkun() {
    clearScreen();
    string user, pass, role, lokasi, spesialisasi;
    while (true) {
        cout << "\n--- FORM REGISTRASI ---\n";
        cout << "Username: "; cin >> user;

        if (usernameExists(user)) {
            cout << "Username '" << user << "' sudah terdaftar. Silakan pilih username lain.\n";
            pauseLayar();
            clearScreen();
        } else {
            break;
        }
    }

    cout << "Password: "; cin >> pass;
    cout << "Role (1.Pelanggan/2.Cleaner): "; cin >> role;
    
    if (role == "2") {
        role = "cleaner";
        cout << "Lokasi: "; cin >> lokasi;
        cout << "Spesialisasi: "; cin >> spesialisasi;
        dbAkun[jmlAkun++] = {user, pass, role, lokasi, spesialisasi, 0};
    } else if (role == "1") {
        role = "pelanggan";
        dbAkun[jmlAkun++] = {user, pass, role, "", "", 0};
    } else {
        cout << "Role tidak valid!\n";
        pauseLayar();
        clearScreen();
        return;
    }
    
    cout << "Registrasi berhasil! Silakan login.\n";
    pauseLayar();
    clearScreen();
}


void inputOrderJasa() {
    clearScreen();
    if (jmlPesanan < 10) {
        dbPesanan[jmlPesanan++] = {nextIdPesanan++, currentUser, "", "Pending", 150000, 2};
        cout << "[CRUD] Order Jasa berhasil dibuat! ID: " << nextIdPesanan - 1 << "\n";
    } else {
        cout << "Kapasitas pesanan penuh!\n";
    }
    pauseLayar();
}

void lihatPesanan() {
    clearScreen();
    cout << "\n--- DAFTAR PESANAN ---\n";
    if (jmlPesanan == 0) { cout << "Belum ada pesanan.\n"; return; }
    for (int i = 0; i < jmlPesanan; i++) {
        cout << "ID: " << dbPesanan[i].id
             << " | Pelanggan: " << dbPesanan[i].pelanggan
             << " | Status: " << dbPesanan[i].status
             << " | Biaya: Rp" << dbPesanan[i].biaya << "\n";
    }
    pauseLayar();
}

void updateProgressKerja() {
    int idPesanan; string statusBaru, konfirmasi;
    clearScreen();
    cout << "\n--- UPDATE PROGRESS ---\n";
    cout << "ID Pesanan: "; cin >> idPesanan;
    if (idPesanan == 0) {
        return;
    }
    cout << "Status Baru (misal: Selesai): "; cin >> statusBaru;
    
    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan) { 
            cout << "Yakin ingin mengubah status pesanan ID " << idPesanan << " menjadi '" << statusBaru << "'? (y/n): ";
            cin >> konfirmasi;
            if (konfirmasi == "y" || konfirmasi == "Y") {
                dbPesanan[i].status = statusBaru; 
                cout << "[CRUD] Progress ID " << idPesanan << " diupdate ke: " << statusBaru << "\n"; 
            } else {
                cout << "Update dibatalkan.\n";
            }
            return; 
        }
    }
    cout << "Pesanan tidak ditemukan.\n";
}

void deleteBooking() {
    int idPesanan; string konfirmasi;
    clearScreen();
    cout << "\n--- HAPUS BOOKING ---\n";
    cout << "Masukkan ID Booking yang dihapus: "; cin >> idPesanan;
    
    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan) {
            cout << "Yakin ingin menghapus booking ID " << idPesanan << "? (y/n): ";
            cin >> konfirmasi;
            if (konfirmasi == "y" || konfirmasi == "Y") {
                for (int j = i; j < jmlPesanan - 1; j++) {
                    dbPesanan[j] = dbPesanan[j + 1];
                }
                jmlPesanan--;
                cout << "[CRUD] Booking ID " << idPesanan << " berhasil dihapus!\n";
            } else {
                cout << "Penghapusan dibatalkan.\n";
            }
            return;
        }
    }
    cout << "Booking tidak ditemukan.\n";
}


void tampilHasilCleaner(int idx[], int n) {
    if (n == 0) { cout << "Tidak ada cleaner yang sesuai.\n"; return; }
    cout << "\n--- HASIL PENCARIAN ---\n";
    for (int i = 0; i < n; i++) {
        Akun& a = dbAkun[idx[i]];
        cout << i+1 << ". " << a.username
             << " | Rating: " << a.rating
             << " | Lokasi: " << a.lokasi << "\n";
    }
}
void cariCleaner() {
    clearScreen();
    int pilihan;
    cout << "\n--- CARI CLEANER ---\n";
    cout << "1. Berdasarkan rating minimum\n";
    cout << "2. Berdasarkan lokasi\n";
    cout << "0. Kembali\n";
    cout << "Pilih: "; cin >> pilihan;

    int hasil[10];
    int jumlahHasil = 0;

    if (pilihan == 1) {
        float minRating;
        cout << "Rating minimum (contoh: 4.0): "; cin >> minRating;
        for (int i = 0; i < jmlAkun; i++) {
            if (dbAkun[i].role == "cleaner" && dbAkun[i].rating >= minRating)
                hasil[jumlahHasil++] = i;
        }

    } 
    else if (pilihan == 2) 
    {
        string kota;
        cout << "Nama kota: "; cin >> kota;
        for (int i = 0; i < jmlAkun; i++) 
        {
            if (dbAkun[i].role == "cleaner" && dbAkun[i].lokasi == kota)
                hasil[jumlahHasil++] = i;
        }

    } 
    else if (pilihan == 0) {
        return;
    } else {
        cout << "Pilihan tidak valid.\n";
        return;
    }

    tampilHasilCleaner(hasil, jumlahHasil);
}

void cetakKuitansi() {
    clearScreen();
    ofstream fileHTML("Kuitansi.html");
    
    if (fileHTML.is_open()) {
        fileHTML << "<!DOCTYPE html>\n";
        fileHTML << "<html>\n";
        fileHTML << "<head>\n";
        fileHTML << "<meta charset='UTF-8'>\n";
        fileHTML << "<title>Kuitansi</title>\n";
        fileHTML << "<style>\n";
        fileHTML << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
        fileHTML << "table { width: 100%; border-collapse: collapse; }\n";
        fileHTML << "th, td { border: 1px solid black; padding: 10px; text-align: left; }\n";
        fileHTML << "th { background-color: #4CAF50; color: white; }\n";
        fileHTML << "h1 { text-align: center; }\n";
        fileHTML << "</style>\n";
        fileHTML << "</head>\n";
        fileHTML << "<body>\n";
        fileHTML << "<h1>=== KUITANSI & SUMMARY ===</h1>\n";
        fileHTML << "<p>Tanggal: " << __DATE__ << " " << __TIME__ << "</p>\n";
        fileHTML << "<table>\n";
        fileHTML << "<tr><th>ID</th><th>Pelanggan</th><th>Cleaner</th><th>Status</th><th>Biaya</th><th>Durasi</th></tr>\n";
        
        if (jmlPesanan == 0) {
            fileHTML << "<tr><td colspan='6' style='text-align:center;'>Belum ada transaksi.</td></tr>\n";
        } else {
            int totalBiaya = 0;
            for (int i = 0; i < jmlPesanan; i++) {
                fileHTML << "<tr>\n";
                fileHTML << "<td>" << dbPesanan[i].id << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].pelanggan << "</td>\n";
                fileHTML << "<td>" << (dbPesanan[i].cleaner.empty() ? "-" : dbPesanan[i].cleaner) << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].status << "</td>\n";
                fileHTML << "<td>Rp" << dbPesanan[i].biaya << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].durasi << " jam</td>\n";
                fileHTML << "</tr>\n";
                totalBiaya += dbPesanan[i].biaya;
            }
            fileHTML << "<tr><th colspan='4'>TOTAL</th><th>Rp" << totalBiaya << "</th><th></th></tr>\n";
        }
        
        fileHTML << "</table>\n";
        fileHTML << "</body>\n";
        fileHTML << "</html>\n";
        fileHTML.close();

        int result = system("wkhtmltopdf Kuitansi.html Kuitansi.pdf");
        
        if (result == 0) {
            cout << "[PRINT] Kuitansi berhasil disimpan ke 'Kuitansi.pdf'\n";
        } else {
            cout << "[ERROR] Gagal convert ke PDF. Pastikan wkhtmltopdf terinstall.\n";
            cout << "Download dari: https://wkhtmltopdf.org/downloads.html\n";
        }
    } else {
        cout << "Gagal membuat file!\n";
    }
    pauseLayar();
}

int main() {
    int pilihan;
    while (true) {
        while (currentUser == "") {
            cout << "\n=== APLIKASI CLEANING SERVICE ===\n";
            cout << "1. Login\n";
            cout << "2. Registrasi\n";
            cout << "0. Keluar Program\n";
            cout << "Pilih menu: ";
            cin >> pilihan;

            switch(pilihan) {
                case 1: 
                    login(); 
                    break;
                case 2: 
                    registerAkun(); 
                    break;
                case 0: 
                    cout << "Keluar dari program...\n"; 
                    return 0;
                default: 
                    cout << "Pilihan tidak valid!\n";
            }
        }

        while (currentUser != "") 
        {
            if (currentRole == "pelanggan") 
            {
                clearScreen();
                cout << "\n=== MENU UTAMA (" << currentRole << ") ===\n";
                cout << "1. Order Jasa (Input)\n";
                cout << "2. Lihat Progress Kerja\n";
                cout << "3. Hapus Booking (Delete)\n";
                cout << "4. Cari Lokasi/Rating Cleaner (Search)\n";
                cout << "5. Cetak Kuitansi (Print)\n";
                cout << "0. Logout\n";
                cout << "Pilih menu: ";
                cin >> pilihan;

                switch(pilihan) 
                {
                    case 1: inputOrderJasa(); break;
                    case 2: lihatPesanan(); break;
                    case 3: deleteBooking(); break;
                    case 4: cariCleaner(); break;
                    case 5: cetakKuitansi(); break;
                    case 0: 
                        cout << "Logout berhasil...\n"; 
                        currentUser = ""; 
                        currentRole = ""; 
                        pauseLayar();
                        clearScreen();
                        break; 
                    default: 
                        cout << "Pilihan tidak valid!\n";
                }
            }
                else if (currentRole == "cleaner") 
                {
                    clearScreen();
                    cout << "\n=== MENU UTAMA (" << currentRole << ") ===\n";
                    cout << "1. Update Progress Kerja\n";
                    cout << "0. Logout\n";
                    cout << "Pilih menu: ";
                    cin >> pilihan;
    
                    switch(pilihan) 
                    {
                        case 1: updateProgressKerja(); break;
                        case 0: 
                            cout << "Logout berhasil...\n"; 
                            currentUser = ""; 
                            currentRole = ""; 
                            pauseLayar();
                            clearScreen();
                            break; 
                        default: 
                            cout << "Pilihan tidak valid!\n";
                    }
                }
        }
        
    }

    return 0;
}
