#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <limits>
#include <conio.h>

using namespace std;

const int MAX_SPESIALISASI = 4;

struct Akun {
    string username, password, role, lokasi;
    string spesialisasi[MAX_SPESIALISASI];
    int hargaPerJam[MAX_SPESIALISASI];
    int jmlSpesialisasi;
    float rating;
};

struct Pesanan {
    int id;
    string pelanggan, cleaner, jasa, status;
    int biayaPerJam, biaya, durasi;
};

Akun dbAkun[100] = {
    {"pelanggan1", "123", "pelanggan", "Bekasi", {}, {}, 0, 0}, 
    {"p", "1", "pelanggan", "Bekasi", {}, {}, 0, 0}, 
    {"c", "1", "cleaner", "Bekasi", {"regular", "deep clean"}, {60000, 100000}, 2, 4.5},
    {"mitra2", "123", "cleaner", "Jakarta", {"deep clean", "carpet cleaning"}, {90000, 50000}, 2, 3.5},
    {"mitra3", "123", "cleaner", "Bandung", {"carpet cleaning", "mattress cleaning"}, {45000, 70000}, 2, 1.5},
    {"mitra4", "123", "cleaner", "Bekasi", {"mattress cleaning", "regular"}, {75000, 55000}, 2, 2.0}
};
//globall variable
string jasa_yang_dipilih; 
string spesialisasi_yang_dipilih;
bool dari_input_jasa = false;
int jmlAkun = 6;

Pesanan dbPesanan[10];
int jmlPesanan = 0, nextIdPesanan = 1;

string currentUser = ""; 
string currentRole = "";

void pauseLayar() {
    cout << "\nTekan Enter untuk lanjut...";
    cin.clear();
    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
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

bool readInt(const string& prompt, int& value) {
    cout << prompt;

    if (cin >> value) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return true;
    }

    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

void tampilMenuJasa() {
    cout << "1. Bersih Rumah Reguler(sebagian)\n";
    cout << "2. Bersih Rumah Deep Clean\n";
    cout << "3. Cuci Karpet\n";
    cout << "4. Cuci Kasur\n";
    cout << "0. Kembali\n";
}

bool pilihJasa(int pilihan, string& namaJasa, string& spesialisasi) {
    switch(pilihan) {
        case 1:
            namaJasa = "Bersih Rumah Reguler(sebagian)";
            spesialisasi = "regular";
            return true;
        case 2:
            namaJasa = "Bersih Rumah Deep Clean";
            spesialisasi = "deep clean";
            return true;
        case 3:
            namaJasa = "Cuci Karpet";
            spesialisasi = "carpet cleaning";
            return true;
        case 4:
            namaJasa = "Cuci Kasur";
            spesialisasi = "mattress cleaning";
            return true;
        default:
            return false;
    }
}

int cariIndexSpesialisasi(const Akun& akun, const string& spesialisasi) {
    for (int i = 0; i < akun.jmlSpesialisasi; i++) {
        if (textFix(akun.spesialisasi[i], spesialisasi)) {
            return i;
        }
    }
    return -1;
}

void clearScreen() {
    system("cls");
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
    string user, pass, role, lokasi;
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
        int jumlahSpesialisasi;
        Akun akunBaru = {user, pass, role, "", {}, {}, 0, 0};
        cout << "Lokasi: "; cin >> lokasi;
        akunBaru.lokasi = lokasi;

        cout << "Jumlah spesialisasi (1-" << MAX_SPESIALISASI << "): ";
        cin >> jumlahSpesialisasi;
        if (jumlahSpesialisasi < 1) jumlahSpesialisasi = 1;
        if (jumlahSpesialisasi > MAX_SPESIALISASI) jumlahSpesialisasi = MAX_SPESIALISASI;

        for (int i = 0; i < jumlahSpesialisasi; i++) {
            int pilihanJasa, harga;
            string namaJasa, spesialisasiDipilih;

            cout << "\nPilih spesialisasi ke-" << i + 1 << ":\n";
            tampilMenuJasa();
            cout << "Pilihan: ";
            cin >> pilihanJasa;

            if (!pilihJasa(pilihanJasa, namaJasa, spesialisasiDipilih)) {
                cout << "Pilihan tidak valid, ulangi.\n";
                i--;
                continue;
            }

            cout << "Harga per jam untuk " << namaJasa << ": Rp";
            cin >> harga;
            akunBaru.spesialisasi[akunBaru.jmlSpesialisasi] = spesialisasiDipilih;
            akunBaru.hargaPerJam[akunBaru.jmlSpesialisasi] = harga;
            akunBaru.jmlSpesialisasi++;
        }

        dbAkun[jmlAkun++] = akunBaru;
    } else if (role == "1") {
        role = "pelanggan";
        dbAkun[jmlAkun++] = {user, pass, role, "", {}, {}, 0, 0};
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




void lihatPesanan() {
    clearScreen();
    cout << "\n--- DAFTAR PESANAN ---\n";
    if (jmlPesanan == 0) { cout << "Belum ada pesanan.\n"; return; }
    for (int i = 0; i < jmlPesanan; i++) {
        cout << "ID: " << dbPesanan[i].id
        << " | Pelanggan: " << dbPesanan[i].pelanggan
        << " | Cleaner: " << dbPesanan[i].cleaner
        << " | Jasa: " << dbPesanan[i].jasa
        << " | Status: " << dbPesanan[i].status
        << " | Durasi: " << dbPesanan[i].durasi << " jam"
        << " | Harga/Jam: Rp" << dbPesanan[i].biayaPerJam
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
        << " | Lokasi: " << a.lokasi;

        if (dari_input_jasa) {
            int idxSpesialisasi = cariIndexSpesialisasi(a, spesialisasi_yang_dipilih);
            if (idxSpesialisasi != -1) {
                cout << " | Harga/Jam: Rp" << a.hargaPerJam[idxSpesialisasi];
            }
        } else {
            cout << " | Spesialisasi: ";
            for (int j = 0; j < a.jmlSpesialisasi; j++) {
                if (j > 0) cout << ", ";
                cout << a.spesialisasi[j] << " (Rp" << a.hargaPerJam[j] << "/jam)";
            }
        }
        cout << "\n";
    }
}

bool spesialisasiSesuai(const Akun& akun) {
    return !dari_input_jasa || cariIndexSpesialisasi(akun, spesialisasi_yang_dipilih) != -1;
}

void buatPesananDariHasil(int idx[], int n) {
    if (!dari_input_jasa || n == 0) {
        return;
    }

    if (jmlPesanan >= 10) {
        cout << "Kapasitas pesanan penuh!\n";
        return;
    }

    int pilihanCleaner;
    if (!readInt("\nPilih cleaner untuk order (0 batal): ", pilihanCleaner)) {
        cout << "Pilihan tidak valid! Harap masukkan angka.\n";
        return;
    }

    if (pilihanCleaner == 0) {
        cout << "Order dibatalkan.\n";
        return;
    }

    if (pilihanCleaner < 1 || pilihanCleaner > n) {
        cout << "Cleaner tidak valid.\n";
        return;
    }

    Akun& cleaner = dbAkun[idx[pilihanCleaner - 1]];
    int idxSpesialisasi = cariIndexSpesialisasi(cleaner, spesialisasi_yang_dipilih);
    if (idxSpesialisasi == -1) {
        cout << "Cleaner tidak memiliki spesialisasi ini.\n";
        return;
    }

    int durasi;
    if (!readInt("Durasi kerja (jam): ", durasi) || durasi <= 0) {
        cout << "Durasi tidak valid.\n";
        return;
    }

    int hargaPerJam = cleaner.hargaPerJam[idxSpesialisasi];
    int totalBiaya = hargaPerJam * durasi;
    dbPesanan[jmlPesanan++] = {
        nextIdPesanan++,
        currentUser,
        cleaner.username,
        jasa_yang_dipilih,
        "Pending",
        hargaPerJam,
        totalBiaya,
        durasi
    };

    cout << "\n[CRUD] Order Jasa berhasil dibuat!\n";
    cout << "ID: " << nextIdPesanan - 1 << "\n";
    cout << "Cleaner: " << cleaner.username << "\n";
    cout << "Jasa: " << jasa_yang_dipilih << "\n";
    cout << "Durasi: " << durasi << " jam\n";
    cout << "Harga/Jam: Rp" << hargaPerJam << "\n";
    cout << "Total Biaya: Rp" << totalBiaya << "\n";
}

void cariCleaner() {
    clearScreen();
    int pilihan;
    cout << "\n--- CARI CLEANER ---\n";
    if (dari_input_jasa) {
        cout << "Jasa dipilih: " << jasa_yang_dipilih << "\n";
        cout << "Spesialisasi dicari: " << spesialisasi_yang_dipilih << "\n";
    }
    cout << "1. Berdasarkan rating minimum\n";
    cout << "2. Berdasarkan lokasi \n";
    cout << "0. Kembali\n";
    if (!readInt("Pilih: ", pilihan)) {
        cout << "Pilihan tidak valid! Harap masukkan angka.\n";
        return;
    }
    
    int hasil[100];
    int jumlahHasil = 0;
    if (pilihan == 1) 
    {
        float minRating;
        cout << "Rating minimum (contoh: 4.0): "; cin >> minRating;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < jmlAkun; i++) {
            if (dbAkun[i].role == "cleaner" && dbAkun[i].rating >= minRating && spesialisasiSesuai(dbAkun[i]))
            hasil[jumlahHasil++] = i;
        }
        
    } 
    
    else if (pilihan == 2) 
    {
        string kota;
        cout << "Nama kota: "; cin >> kota;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < jmlAkun; i++) 
        {
            if (dbAkun[i].role == "cleaner" && dbAkun[i].lokasi == kota && spesialisasiSesuai(dbAkun[i]))
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
    buatPesananDariHasil(hasil, jumlahHasil);
    pauseLayar();
}

void inputOrderJasa() {
    clearScreen();
    int pilihan;
    cout << "\n--- ORDER JASA ---\n";
    tampilMenuJasa();
    if (!readInt("Masukkan pilihan: ", pilihan)) {
        cout << "Pilihan tidak valid! Harap masukkan angka.\n";
        pauseLayar();
        return;
    }

    if (pilihan == 0) {
        return;
    }

    if (!pilihJasa(pilihan, jasa_yang_dipilih, spesialisasi_yang_dipilih)) {
        cout << "Pilihan tidak valid!\n";
        pauseLayar();
        return;
    }

    dari_input_jasa = true;
    cariCleaner();
    dari_input_jasa = false;
    jasa_yang_dipilih = "";
    spesialisasi_yang_dipilih = "";
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
        fileHTML << "<tr><th>ID</th><th>Pelanggan</th><th>Cleaner</th><th>Jasa</th><th>Status</th><th>Harga/Jam</th><th>Biaya</th><th>Durasi</th></tr>\n";
        
        if (jmlPesanan == 0) {
            fileHTML << "<tr><td colspan='8' style='text-align:center;'>Belum ada transaksi.</td></tr>\n";
        } else {
            int totalBiaya = 0;
            for (int i = 0; i < jmlPesanan; i++) {
                fileHTML << "<tr>\n";
                fileHTML << "<td>" << dbPesanan[i].id << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].pelanggan << "</td>\n";
                fileHTML << "<td>" << (dbPesanan[i].cleaner.empty() ? "-" : dbPesanan[i].cleaner) << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].jasa << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].status << "</td>\n";
                fileHTML << "<td>Rp" << dbPesanan[i].biayaPerJam << "</td>\n";
                fileHTML << "<td>Rp" << dbPesanan[i].biaya << "</td>\n";
                fileHTML << "<td>" << dbPesanan[i].durasi << " jam</td>\n";
                fileHTML << "</tr>\n";
                totalBiaya += dbPesanan[i].biaya;
            }
            fileHTML << "<tr><th colspan='6'>TOTAL</th><th>Rp" << totalBiaya << "</th><th></th></tr>\n";
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
            cout << "\n=========================\n";
            cout << "SELAMAT DATANG DI GOCLEAN";
            cout << "\n=========================\n";
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
                // clearScreen();
                cout << "\n=== MENU UTAMA (" << currentRole << ") ===\n";
                cout << "1. Order Jasa (Input)\n";
                cout << "2. Lihat Progress Kerja\n";
                cout << "3. Hapus Booking (Delete)\n";
                cout << "4. Cari Lokasi/Rating Cleaner (Search)\n";
                cout << "5. Cetak Kuitansi (Print)\n";
                cout << "0. Logout\n";
                if (!readInt("Pilih menu: ", pilihan)) {
                    cout << "Pilihan tidak valid! Harap masukkan angka.\n";
                    continue;
                }

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
                    if (!readInt("Pilih menu: ", pilihan)) {
                        cout << "Pilihan tidak valid! Harap masukkan angka.\n";
                        continue;
                    }
    
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
