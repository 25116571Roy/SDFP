#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <chrono>
#include <thread>
#include <limits>
#include <vector>
#include <conio.h>
#include <iomanip>
#include <sstream>
using namespace std;

const int MAX_SPESIALISASI = 4;

struct Akun {
    string username, password, role, lokasi, region;
    string spesialisasi[MAX_SPESIALISASI];
    int hargaPerJam[MAX_SPESIALISASI];
    int jmlSpesialisasi;
    float rating;
};

struct Pesanan {
    int id;
    string pelanggan, cleaner, jasa, status, petugas;
    int biayaPerJam, biaya, durasi, bayar, kembalian;
};

Akun dbAkun[100] = {
    {"pelanggan1", "123", "pelanggan", "Bekasi", "", {}, {}, 0, 0},
    {"p", "1", "pelanggan", "Bekasi", "", {}, {}, 0, 0},
    {"c", "1", "cleaner", "Bekasi", "jawabarat", {"regular", "deep clean"}, {60000, 100000}, 2, 4.5},
    {"mitra2", "123", "cleaner", "Mojokerto", "jawatengah", {"deep clean", "carpet cleaning"}, {90000, 50000}, 2, 3.5},
    {"mitra3", "123", "cleaner", "Surabaya", "jawatimur", {"carpet cleaning", "mattress cleaning"}, {45000, 70000}, 2, 1.5},
    {"mitra4", "123", "cleaner", "jakarta", "jakarta", {"mattress cleaning", "regular"}, {75000, 55000}, 2, 2.0}
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
    system("pause");
    cin.clear();
    
}

string toLower(string teks) {
    for (char& c : teks) {
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    }
    return teks;
}

string formatFloat(float value, int precision = 2) {
    stringstream ss;
    ss << fixed << setprecision(precision) << value;
    return ss.str();
}

bool usernameExists(const string& user) {
    for (int i = 0; i < jmlAkun; i++) {
        if (dbAkun[i].username == user) {
            return true;
        }
    }
    return false;
}

int cariIndexAkun(const string& username) {
    for (int i = 0; i < jmlAkun; i++) {
        if (dbAkun[i].username == username) {
            return i;
        }
    }
    return -1;
}

bool readInt(const string& prompt, int& value) {
    cout << prompt;

    if (cin >> value) {
        cin.ignore(1000, '\n');
        return true;
    }

    cin.clear();
    cin.ignore(1000, '\n');
    return false;
}

bool bolehAksesPesanan(const Pesanan& pesanan) {
    if (currentRole == "pelanggan") {
        return pesanan.pelanggan == currentUser;
    }

    if (currentRole == "cleaner") {
        return pesanan.cleaner == currentUser;
    }

    return false;
}

vector<size_t> hitungLebarKolom(const vector<string>& headers, const vector<vector<string>>& rows) {
    vector<size_t> widths(headers.size());
    for (size_t i = 0; i < headers.size(); i++) {
        widths[i] = headers[i].length();
    }

    for (const auto& row : rows) {
        for (size_t i = 0; i < row.size(); i++) {
            widths[i] = max(widths[i], row[i].length());
        }
    }

    return widths;
}

void cetakGarisBatas(const vector<size_t>& widths) {
    cout << "+";
    for (size_t width : widths) {
        cout << string(width + 2, '-') << "+";
    }
    cout << "\n";
}

void cetakBaris(const vector<string>& cells, const vector<size_t>& widths) {
    cout << "|";
    for (size_t i = 0; i < cells.size(); i++) {
        cout << " " << left << setw(static_cast<int>(widths[i])) << cells[i] << " |";
    }
    cout << "\n";
}

void tampilTabel(const vector<string>& headers, const vector<vector<string>>& rows) {
    if (headers.empty()) {
        return;
    }

    vector<size_t> widths = hitungLebarKolom(headers, rows);
    cetakGarisBatas(widths);
    cetakBaris(headers, widths);
    cetakGarisBatas(widths);

    for (const auto& row : rows) {
        cetakBaris(row, widths);
    }

    cetakGarisBatas(widths);
}

void tampilTabelPesanan(const int indices[], int count) {
    vector<string> headers = {"ID", "Pelanggan", "Cleaner", "Jasa", "Status", "Durasi", "Harga/Jam", "Biaya"};
    vector<vector<string>> rows;
    rows.reserve(static_cast<size_t>(count));

    for (int i = 0; i < count; i++) {
        const Pesanan& p = dbPesanan[indices[i]];
        rows.push_back({
            to_string(p.id),
            p.pelanggan,
            p.cleaner,
            p.jasa,
            p.status,
            to_string(p.durasi) + " jam",
            string("Rp") + to_string(p.biayaPerJam),
            string("Rp") + to_string(p.biaya)
        });
    }

    tampilTabel(headers, rows);
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
    string spesialisasiLower = toLower(spesialisasi);
    for (int i = 0; i < akun.jmlSpesialisasi; i++) {
        if (toLower(akun.spesialisasi[i]) == spesialisasiLower) {
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
        Akun akunBaru = {user, pass, role, "", "", {}, {}, 0, 0.0f};
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
        dbAkun[jmlAkun++] = {user, pass, role, "", "", {}, {}, 0, 0.0f};
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
    int indices[10];
    int count = 0;

    for (int i = 0; i < jmlPesanan; i++) {
        if (bolehAksesPesanan(dbPesanan[i])) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "Belum ada pesanan.\n";
    } else {
        tampilTabelPesanan(indices, count);
    }

    pauseLayar();
}

void lihatpesananCleaner() {
    clearScreen();
    cout << "\n--- DAFTAR PESANAN ---\n";
    int indices[10];
    int count = 0;

    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].cleaner == currentUser) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "belum ada pesanan\n";
    } else {
        tampilTabelPesanan(indices, count);
    }
    pauseLayar();
}

void tampilSpesialisasiCleaner(const Akun& cleaner) {
    if (cleaner.jmlSpesialisasi == 0) {
        cout << "Belum ada spesialisasi.\n";
        return;
    }

    vector<string> headers = {"No", "Spesialisasi", "Harga/Jam"};
    vector<vector<string>> rows;
    rows.reserve(static_cast<size_t>(cleaner.jmlSpesialisasi));

    for (int i = 0; i < cleaner.jmlSpesialisasi; i++) {
        rows.push_back({
            to_string(i + 1),
            cleaner.spesialisasi[i],
            "Rp" + to_string(cleaner.hargaPerJam[i])
        });
    }

    tampilTabel(headers, rows);
}

void updateJasa() {
    int indexAkun = cariIndexAkun(currentUser);
    if (indexAkun == -1 || dbAkun[indexAkun].role != "cleaner") {
        cout << "Akun cleaner tidak ditemukan.\n";
        pauseLayar();
        return;
    }

    Akun& cleaner = dbAkun[indexAkun];
    int pilihan;

    while (true) {
        clearScreen();
        cout << "\n--- UPDATE JASA ---\n";
        cout << "Cleaner: " << cleaner.username << "\n";
        cout << "Lokasi: " << cleaner.lokasi << "\n\n";
        tampilSpesialisasiCleaner(cleaner);
        cout << "\n1. Tambah spesialisasi\n";
        cout << "2. Hapus spesialisasi\n";
        cout << "3. Edit harga jasa\n";
        cout << "0. Kembali\n";

        if (!readInt("Pilih: ", pilihan)) {
            cout << "Pilihan tidak valid! Harap masukkan angka.\n";
            pauseLayar();
            continue;
        }

        if (pilihan == 0) {
            return;
        }

        if (pilihan == 1) {
            if (cleaner.jmlSpesialisasi >= MAX_SPESIALISASI) {
                cout << "Spesialisasi sudah penuh. Maksimal " << MAX_SPESIALISASI << ".\n";
                pauseLayar();
                continue;
            }

            int pilihanJasa, harga;
            string namaJasa, spesialisasiDipilih;

            cout << "\nPilih spesialisasi baru:\n";
            tampilMenuJasa();
            if (!readInt("Pilihan: ", pilihanJasa) || pilihanJasa == 0) {
                cout << "Tambah spesialisasi dibatalkan.\n";
                pauseLayar();
                continue;
            }

            if (!pilihJasa(pilihanJasa, namaJasa, spesialisasiDipilih)) {
                cout << "Pilihan jasa tidak valid.\n";
                pauseLayar();
                continue;
            }

            if (cariIndexSpesialisasi(cleaner, spesialisasiDipilih) != -1) {
                cout << "Spesialisasi ini sudah ada.\n";
                pauseLayar();
                continue;
            }

            if (!readInt("Harga per jam: Rp", harga) || harga <= 0) {
                cout << "Harga tidak valid.\n";
                pauseLayar();
                continue;
            }

            cleaner.spesialisasi[cleaner.jmlSpesialisasi] = spesialisasiDipilih;
            cleaner.hargaPerJam[cleaner.jmlSpesialisasi] = harga;
            cleaner.jmlSpesialisasi++;
            cout << "Spesialisasi " << namaJasa << " berhasil ditambahkan.\n";
            pauseLayar();
        } else if (pilihan == 2) {
            if (cleaner.jmlSpesialisasi == 0) {
                cout << "Tidak ada spesialisasi untuk dihapus.\n";
                pauseLayar();
                continue;
            }

            int nomor;
            if (!readInt("Nomor spesialisasi yang dihapus: ", nomor) || nomor < 1 || nomor > cleaner.jmlSpesialisasi) {
                cout << "Nomor spesialisasi tidak valid.\n";
                pauseLayar();
                continue;
            }

            int indexHapus = nomor - 1;
            string spesialisasiHapus = cleaner.spesialisasi[indexHapus];
            for (int i = indexHapus; i < cleaner.jmlSpesialisasi - 1; i++) {
                cleaner.spesialisasi[i] = cleaner.spesialisasi[i + 1];
                cleaner.hargaPerJam[i] = cleaner.hargaPerJam[i + 1];
            }
            cleaner.jmlSpesialisasi--;
            cleaner.spesialisasi[cleaner.jmlSpesialisasi] = "";
            cleaner.hargaPerJam[cleaner.jmlSpesialisasi] = 0;
            cout << "Spesialisasi " << spesialisasiHapus << " berhasil dihapus.\n";
            pauseLayar();
        } else if (pilihan == 3) {
            if (cleaner.jmlSpesialisasi == 0) {
                cout << "Tidak ada spesialisasi untuk diedit.\n";
                pauseLayar();
                continue;
            }

            int nomor, hargaBaru;
            if (!readInt("Nomor spesialisasi yang diedit: ", nomor) || nomor < 1 || nomor > cleaner.jmlSpesialisasi) {
                cout << "Nomor spesialisasi tidak valid.\n";
                pauseLayar();
                continue;
            }

            if (!readInt("Harga baru per jam: Rp", hargaBaru) || hargaBaru <= 0) {
                cout << "Harga tidak valid.\n";
                pauseLayar();
                continue;
            }

            cleaner.hargaPerJam[nomor - 1] = hargaBaru;
            cout << "Harga " << cleaner.spesialisasi[nomor - 1] << " berhasil diubah menjadi Rp" << hargaBaru << ".\n";
            pauseLayar();
        } else {
            cout << "Pilihan tidak valid.\n";
            pauseLayar();
        }
    }
}

void updateProgressKerja() {
    int idPesanan; string statusBaru, konfirmasi, petugas;
    clearScreen();
    cout << "\n--- DAFTAR PESANAN ---\n";
    int indices[10];
    int count = 0;

    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].cleaner == currentUser) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "belum ada pesanan\n";
    } else {
        tampilTabelPesanan(indices, count);
    }
    cout << "\n--- UPDATE PROGRESS ---\n";

    if (count == 0) {
        cout << "belum ada pesanan\n";
        pauseLayar();
        return;
    }

    cout << "ID Pesanan: "; cin >> idPesanan;
    if (idPesanan == 0) {
        return;
    }
    cout << "Status Baru (misal: Selesai): "; cin >> statusBaru;
    cin.ignore(1000, '\n');
    cout << "Nama Petugas: "; getline(cin, petugas);
    
    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan && bolehAksesPesanan(dbPesanan[i])) { 
            cout << "Yakin ingin mengubah status pesanan ID " << idPesanan << " menjadi '" << statusBaru << "'? (y/n): ";
            string konfirmasiInput;
            getline(cin, konfirmasiInput);
            if (konfirmasiInput == "y" || konfirmasiInput == "Y") {
                dbPesanan[i].status = statusBaru;
                dbPesanan[i].petugas = petugas;
                cout << "[CRUD] Progress ID " << idPesanan << " diupdate ke: " << statusBaru << "\n";
                cout << "\n========== PROSES UPDATE SELESAI ==========\n";
                cout << "ID Pesanan: " << idPesanan << "\n";
                cout << "Status Baru: " << dbPesanan[i].status << "\n";
                cout << "Petugas: " << dbPesanan[i].petugas << "\n";
                cout << "==========================================\n";
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

    int indices[10];
    int count = 0;

    for (int i = 0; i < jmlPesanan; i++) {
        if (bolehAksesPesanan(dbPesanan[i])) {
            indices[count++] = i;
        }
    }

    if (count == 0) {
        cout << "belum memiliki pesanan\n";
        pauseLayar();
        return;
    }

    tampilTabelPesanan(indices, count);

    cout << "Masukkan ID Booking yang dihapus: "; cin >> idPesanan;

    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan && bolehAksesPesanan(dbPesanan[i])) {
            if (toLower(dbPesanan[i].status) != "pending") {
                cout << "Booking hanya bisa dihapus jika status masih Pending.\n";
                cout << "Status saat ini: " << dbPesanan[i].status << "\n";
                pauseLayar();
                return;
            }

            cout << "Yakin ingin menghapus booking ID " << idPesanan << "? (y/n): ";
            cin >> konfirmasi;
            if (konfirmasi == "y" || konfirmasi == "Y") {
                int jumlahBayar = dbPesanan[i].bayar;
                for (int j = i; j < jmlPesanan - 1; j++) {
                    dbPesanan[j] = dbPesanan[j + 1];
                }
                jmlPesanan--;
                cout << "[CRUD] Booking ID " << idPesanan << " berhasil dihapus!\n";
                cout << "Uang telah dikembalikan: Rp" << jumlahBayar << "\n";
            } else {
                cout << "Penghapusan dibatalkan.\n";
            }
            return;
        }
    }
    cout << "Booking tidak ditemukan.\n";
}


void tampilHasilCleaner(int idx[], int n) {
    clearScreen();
    if (n == 0) { cout << "Tidak ada cleaner yang sesuai.\n"; return; }
    cout << "\n--- HASIL PENCARIAN ---\n";

    vector<string> headers = {"No", "Username", "Rating", "Lokasi", "Spesialisasi", "Harga/Jam"};
    vector<vector<string>> rows;
    rows.reserve(static_cast<size_t>(n));

    for (int i = 0; i < n; i++) {
        Akun& a = dbAkun[idx[i]];
        string spesialisasiStr = "";
        string hargaStr = "";

        if (dari_input_jasa) {
            int idxSpesialisasi = cariIndexSpesialisasi(a, spesialisasi_yang_dipilih);
            if (idxSpesialisasi != -1) {
                spesialisasiStr = spesialisasi_yang_dipilih;
                hargaStr = "Rp" + to_string(a.hargaPerJam[idxSpesialisasi]);
            }
        } else {
            for (int j = 0; j < a.jmlSpesialisasi; j++) {
                if (j > 0) spesialisasiStr += ", ";
                spesialisasiStr += a.spesialisasi[j];
                if (j > 0) hargaStr += ", ";
                hargaStr += "Rp" + to_string(a.hargaPerJam[j]);
            }
        }

        rows.push_back({
            to_string(i + 1),
            a.username,
            formatFloat(a.rating),
            a.lokasi,
            spesialisasiStr,
            hargaStr
        });
    }

    tampilTabel(headers, rows);
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
    if (!readInt("Durasi kerja (jam): ", durasi) || durasi <= 0 || durasi > 72) {
        cout << "Durasi tidak valid.\n";
        return;
    }

    int hargaPerJam = cleaner.hargaPerJam[idxSpesialisasi];
    int totalBiaya = hargaPerJam * durasi;
    
    cout << "Total biaya: Rp" << totalBiaya << "\n";
    int jumlahBayar;
    if (!readInt("Jumlah pembayaran (Rp): ", jumlahBayar) || jumlahBayar < totalBiaya) {
        cout << "Pembayaran tidak valid. Minimal: Rp" << totalBiaya << "\n";
        return;
    }
    
    int kembalian = jumlahBayar - totalBiaya;
    
    dbPesanan[jmlPesanan++] = {
        nextIdPesanan++,
        currentUser,
        cleaner.username,
        jasa_yang_dipilih,
        "Pending",
        "",
        hargaPerJam,
        totalBiaya,
        durasi,
        jumlahBayar,
        kembalian
    };

    cout << "\n[CRUD] Order Jasa berhasil dibuat!\n";
    cout << "ID: " << nextIdPesanan - 1 << "\n";
    cout << "Cleaner: " << cleaner.username << "\n";
    cout << "Jasa: " << jasa_yang_dipilih << "\n";
    cout << "Durasi: " << durasi << " jam\n";
    cout << "Harga/Jam: Rp" << hargaPerJam << "\n";
    cout << "Total Biaya: Rp" << totalBiaya << "\n";
    cout << "Jumlah Bayar: Rp" << jumlahBayar << "\n";
    cout << "Kembalian: Rp" << kembalian << "\n";
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
        cout << "Rating minimum (0.0 - 5.0): "; cin >> minRating;
        cin.ignore(1000, '\n');
        for (int i = 0; i < jmlAkun; i++) {
            if (dbAkun[i].role == "cleaner" && dbAkun[i].rating >= minRating && spesialisasiSesuai(dbAkun[i]))
            hasil[jumlahHasil++] = i;
        }
    } 
    
    else if (pilihan == 2)
    {
        clearScreen();
        int pilihanRegion;
        string region;
        cout << "Pilih Region:\n";
        cout << "1. Jawa Barat\n";
        cout << "2. Jawa Tengah\n";
        cout << "3. Jawa Timur\n";
        cout << "4. Jakarta\n";
        cout << "Pilihan: "; cin >> pilihanRegion;
        switch (pilihanRegion) {
            case 1: region = "jawabarat"; break;
            case 2: region = "jawatengah"; break;
            case 3: region = "jawatimur"; break;
            case 4: region = "jakarta"; break;
            default:
                cout << "Pilihan tidak valid.\n";
                return;
        }
        for (int i = 0; i < jmlAkun; i++) 
        {
            if (dbAkun[i].role == "cleaner" && toLower(dbAkun[i].region) == region && spesialisasiSesuai(dbAkun[i]))
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
    clearScreen();
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
    int idPesanan;
    int indexPesanan = -1;

    if (jmlPesanan == 0) {
        cout << "Belum ada transaksi untuk dicetak.\n";
        pauseLayar();
        clearScreen();
        return;
    }
    cout << "\n--- CETAK KUITANSI ---\n";

    if (!readInt("Masukkan ID Pesanan yang ingin dicetak: ", idPesanan)) {
        cout << "ID tidak valid! Harap masukkan angka.\n";
        pauseLayar();
        return;
    }

    for (int i = 0; i < jmlPesanan; i++) {
        if (dbPesanan[i].id == idPesanan && bolehAksesPesanan(dbPesanan[i])) {
            indexPesanan = i;
            break;
        }
    }

    if (indexPesanan == -1) {
        cout << "Pesanan tidak ditemukan.\n";
        pauseLayar();
        return;
    }

    if (toLower(dbPesanan[indexPesanan].status) != "selesai") {
        cout << "Kuitansi hanya bisa dicetak jika status pesanan sudah Selesai.\n";
        cout << "Status pesanan saat ini: " << dbPesanan[indexPesanan].status << "\n";
        pauseLayar();
        return;
    }
    ofstream fileHTML("Kuitansi.html");
    if (fileHTML.is_open()) {
        Pesanan& pesanan = dbPesanan[indexPesanan];
        int kembalian = pesanan.bayar - pesanan.biaya;
        
        fileHTML << "<!DOCTYPE html>\n";
        fileHTML << "<html>\n";
        fileHTML << "<head>\n";
        fileHTML << "<meta charset='UTF-8'>\n";
        fileHTML << "<title>Kuitansi Transaksi</title>\n";
        fileHTML << "<style>\n";
        fileHTML << "body { font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }\n";
        fileHTML << ".kuitansi { background-color: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); max-width: 600px; margin: 0 auto; }\n";
        fileHTML << "h1 { text-align: center; color: #333; border-bottom: 3px solid #4CAF50; padding-bottom: 15px; }\n";
        fileHTML << "h2 { color: #4CAF50; border-top: 2px solid #ddd; padding-top: 15px; margin-top: 20px; }\n";
        fileHTML << ".info-row { display: flex; justify-content: space-between; padding: 8px 0; border-bottom: 1px dotted #ddd; }\n";
        fileHTML << ".label { font-weight: bold; color: #555; }\n";
        fileHTML << ".value { color: #333; }\n";
        fileHTML << ".section-total { background-color: #f9f9f9; padding: 15px; border-radius: 5px; margin: 15px 0; }\n";
        fileHTML << ".total-row { display: flex; justify-content: space-between; font-size: 16px; font-weight: bold; margin: 5px 0; }\n";
        fileHTML << ".kembalian { color: #27ae60; }\n";
        fileHTML << ".tanggal { text-align: center; color: #999; font-size: 12px; margin-top: 20px; }\n";
        fileHTML << "</style>\n";
        fileHTML << "</head>\n";
        fileHTML << "<body>\n";
        fileHTML << "<div class='kuitansi'>\n";
        fileHTML << "<h1>KUITANSI TRANSAKSI</h1>\n";
        
        fileHTML << "<h2>Informasi Transaksi</h2>\n";
        fileHTML << "<div class='info-row'><span class='label'>ID Pesanan:</span><span class='value'>#" << pesanan.id << "</span></div>\n";
        fileHTML << "<div class='info-row'><span class='label'>Status:</span><span class='value'>" << pesanan.status << "</span></div>\n";
        
        fileHTML << "<h2>Data Pemesan & Penyedia Jasa</h2>\n";
        fileHTML << "<div class='info-row'><span class='label'>Pelanggan:</span><span class='value'>" << pesanan.pelanggan << "</span></div>\n";
        fileHTML << "<div class='info-row'><span class='label'>Cleaner/Penyedia:</span><span class='value'>" << (pesanan.cleaner.empty() ? "-" : pesanan.cleaner) << "</span></div>\n";
        fileHTML << "<div class='info-row'><span class='label'>Petugas Penyelesai:</span><span class='value'>" << (pesanan.petugas.empty() ? "-" : pesanan.petugas) << "</span></div>\n";
        
        fileHTML << "<h2>Detail Layanan</h2>\n";
        fileHTML << "<div class='info-row'><span class='label'>Jenis Jasa:</span><span class='value'>" << pesanan.jasa << "</span></div>\n";
        fileHTML << "<div class='info-row'><span class='label'>Durasi Kerja:</span><span class='value'>" << pesanan.durasi << " jam</span></div>\n";
        fileHTML << "<div class='info-row'><span class='label'>Harga per Jam:</span><span class='value'>Rp" << fixed << setw(10) << pesanan.biayaPerJam << "</span></div>\n";
        
        fileHTML << "<h2>Rincian Biaya</h2>\n";
        fileHTML << "<div class='section-total'>\n";
        fileHTML << "<div class='total-row'><span>Total Biaya (Durasi x Harga/Jam):</span><span>Rp" << fixed << setw(10) << pesanan.biaya << "</span></div>\n";
        fileHTML << "<div class='total-row' style='color: #e74c3c;'><span>Jumlah Pembayaran:</span><span>Rp" << fixed << setw(10) << pesanan.bayar << "</span></div>\n";
        fileHTML << "<div class='total-row kembalian'><span>Sisa/Kembalian:</span><span>Rp" << fixed << setw(10) << (kembalian >= 0 ? kembalian : 0) << "</span></div>\n";
        fileHTML << "</div>\n";
        
        fileHTML << "<div class='tanggal'>Tanggal Cetak: " << __DATE__ << " " << __TIME__ << "</div>\n";
        fileHTML << "</div>\n";
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
    clearScreen();
    int pilihan;
    while (true) {
        while (currentUser == "") {
            cout << "\n=========================\n";
            cout << "SELAMAT DATANG DI GOCLEAN";
            cout << "\n=========================\n";
            cout << "1. Login\n";
            cout << "2. Registrasi\n";
            cout << "0. Keluar Program\n";
            if (!readInt("Pilih menu: ", pilihan)) {
                cout << "Pilihan tidak valid! Harap masukkan angka.\n";
                pauseLayar();
                clearScreen();
                continue;
            }

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
                    pauseLayar();
                    clearScreen();
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
                    cout << "2. Lihat List Pesanan\n";
                    cout << "3. Cetak Kuitansi\n";
                    cout << "4. Update Jasa\n";
                    cout << "0. Logout\n";
                    if (!readInt("Pilih menu: ", pilihan)) {
                        cout << "Pilihan tidak valid! Harap masukkan angka.\n";
                        continue;
                    }
    
                    switch(pilihan) 
                    {
                        case 1: updateProgressKerja(); break;
                        case 2: lihatpesananCleaner(); break;
                        case 3: cetakKuitansi(); break;
                        case 4: updateJasa(); break;
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
