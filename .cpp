#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>

using namespace std;

struct Akun { string username, password, role, lokasi; float rating; };
struct Pesanan { int id; string pelanggan, cleaner, status; int biaya, durasi; };

Akun dbAkun[10] = {
    {"pelanggan1", "123", "pelanggan", "Bekasi", 0}, 
    {"mitra1", "123", "cleaner", "Bekasi", 4.8}
};
int jmlAkun = 2;

Pesanan dbPesanan[10];
int jmlPesanan = 0, nextIdPesanan = 1;

string currentUser = ""; 
string currentRole = "";

void login() {
    string user, pass;
    cout << "\n--- FORM LOGIN ---\n";
    cout << "Username: "; cin >> user;
    cout << "Password: "; cin >> pass;
    
    for (int i = 0; i < jmlAkun; i++) {
        if (dbAkun[i].username == user && dbAkun[i].password == pass) {
            currentUser = user; 
            currentRole = dbAkun[i].role;
            cout << "Login Sukses! Selamat datang, " << currentUser << " (" << currentRole << ")\n"; 
            return;
        }
    }
    cout << "Login Gagal! Username atau password salah.\n";
}

void clearScreen() {
    system("cls");
    cout.flush();
    this_thread::sleep_for(chrono::milliseconds(50));
}

void inputOrderJasa() {
    clearScreen();
    if (jmlPesanan < 10) {
        dbPesanan[jmlPesanan++] = {nextIdPesanan++, currentUser, "", "Pending", 150000, 2};
        cout << "[CRUD] Order Jasa berhasil dibuat! ID: " << nextIdPesanan - 1 << "\n";
    } else {
        cout << "Kapasitas pesanan penuh!\n";
    }
}

void updateProgressKerja() {
    int idPesanan; string statusBaru;
    clearScreen();
    cout << "\n--- UPDATE PROGRESS ---\n";
    cout << "ID Pesanan: "; cin >> idPesanan;
    cout << "Status Baru (misal: Selesai): "; cin >> statusBaru;
    
    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan) { 
            dbPesanan[i].status = statusBaru; 
            cout << "[CRUD] Progress ID " << idPesanan << " diupdate ke: " << statusBaru << "\n"; 
            return; 
        }
    }
    cout << "Pesanan tidak ditemukan.\n";
}

void deleteBooking() {
    int idPesanan;
    clearScreen();
    cout << "\n--- HAPUS BOOKING ---\n";
    cout << "Masukkan ID Booking yang dihapus: "; cin >> idPesanan;
    
    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan) {
            for (int j = i; j < jmlPesanan - 1; j++) {
                dbPesanan[j] = dbPesanan[j + 1];
            }
            jmlPesanan--;
            cout << "[CRUD] Booking ID " << idPesanan << " berhasil dihapus!\n";
            return;
        }
    }
    cout << "Booking tidak ditemukan.\n";
}

void cariCleaner() {
    clearScreen();
    cout << "\n--- CARI CLEANER ---\n";
    cout << "[SEARCH] Cleaner Terdekat: mitra1 (Rating: 4.8, Lokasi: Bekasi)\n";
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
        
        // Convert HTML to PDF using wkhtmltopdf
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
}

int main() {
    int pilihan;
    while (true) {
        while (currentUser == "") {
            cout << "\n=== APLIKASI CLEANING SERVICE ===\n";
            cout << "1. Login\n";
            cout << "0. Keluar Program\n";
            cout << "Pilih menu: ";
            cin >> pilihan;

            switch(pilihan) {
                case 1: 
                    login(); 
                    break;
                case 0: 
                    cout << "Keluar dari program...\n"; 
                    return 0;
                default: 
                    cout << "Pilihan tidak valid!\n";
            }
        }

        while (currentUser != "") {
            cout << "\n=== MENU UTAMA (" << currentRole << ") ===\n";
            cout << "1. Order Jasa (Input)\n";
            cout << "2. Update Progress Kerja\n";
            cout << "3. Hapus Booking (Delete)\n";
            cout << "4. Cari Lokasi/Rating Cleaner (Search)\n";
            cout << "5. Cetak Kuitansi (Print)\n";
            cout << "0. Logout\n";
            cout << "Pilih menu: ";
            cin >> pilihan;

            switch(pilihan) {
                case 1: inputOrderJasa(); break;
                case 2: updateProgressKerja(); break;
                case 3: deleteBooking(); break;
                case 4: cariCleaner(); break;
                case 5: cetakKuitansi(); break;
                case 0: 
                    cout << "Logout berhasil...\n"; 
                    currentUser = ""; 
                    currentRole = ""; 
                    break; 
                default: 
                    cout << "Pilihan tidak valid!\n";
            }
        }
        
    }

    return 0;
}