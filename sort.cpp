#include <algorithm>
#include <chrono>
#include <cctype>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std;
const int Height = 18;
const int SpeedDelay[10] = {0, 140, 90, 55, 34, 20, 12, 7, 3, 0};
const string Names[10] = {"", "Bubble Sort", "Selection Sort", "Insertion Sort", "Shell Sort", "Merge Sort", "Quick Sort", "Heap Sort", "Counting Sort", "Radix Sort"};
int N = 24, Speed = 6, DelayMs = 12;
vector<int> A;
long long Cmp, Mov;
string AlgoName, Status;
vector<int> Hot;
int HotKind;
mt19937 Rng{static_cast<unsigned>(chrono::steady_clock::now().time_since_epoch().count())};
void EnableAnsi() {
#ifdef _WIN32
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD Mode = 0;
	if (GetConsoleMode(Handle, &Mode)) SetConsoleMode(Handle, Mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
#endif
}
void RestoreTerminal() {
	cout << "\033[0m\033[?25h\033[?1049l" << flush;
}
int RandomInt(int Lo, int Hi) {
	uniform_int_distribution<int> Dist(Lo, Hi);
	return Dist(Rng);
}
int BarWidth() {
	if (N <= 18) return 4;
	if (N <= 26) return 3;
	return 2;
}
string BarColor(int Value, int Top) {
	static const int Stop[5][3] = {{0, 175, 255}, {0, 230, 150}, {160, 230, 40}, {255, 200, 0}, {255, 80, 60}};
	double Ratio = Top <= 1 ? 0.0 : static_cast<double>(Value - 1) / static_cast<double>(Top - 1);
	Ratio = max(0.0, min(1.0, Ratio));
	double Pos = Ratio * 4.0;
	int Seg = min(3, static_cast<int>(Pos));
	double Frac = Pos - Seg;
	int R = static_cast<int>(Stop[Seg][0] + (Stop[Seg + 1][0] - Stop[Seg][0]) * Frac);
	int G = static_cast<int>(Stop[Seg][1] + (Stop[Seg + 1][1] - Stop[Seg][1]) * Frac);
	int B = static_cast<int>(Stop[Seg][2] + (Stop[Seg + 1][2] - Stop[Seg][2]) * Frac);
	int Qr = max(0, min(5, (R * 5 + 127) / 255));
	int Qg = max(0, min(5, (G * 5 + 127) / 255));
	int Qb = max(0, min(5, (B * 5 + 127) / 255));
	return "\033[48;5;" + to_string(16 + 36 * Qr + 6 * Qg + Qb) + "m";
}
string HotColor(int Kind) {
	if (Kind == 1) return "\033[48;5;231m";
	if (Kind == 2) return "\033[48;5;226m";
	if (Kind == 3) return "\033[48;5;51m";
	if (Kind == 4) return "\033[48;5;201m";
	return "\033[48;5;245m";
}
bool IsHot(int Index) {
	for (int x : Hot) if (x == Index) return true;
	return false;
}
void Draw() {
	int Top = *max_element(A.begin(), A.end());
	int Width = BarWidth();
	ostringstream Out;
	Out << "\033[H";
	Out << "  \033[1;97m" << AlgoName << "\033[0m   \033[90mn " << N << "   speed " << Speed << "/9   cmp " << Cmp << "   mov " << Mov << "\033[0m\033[K\n";
	string StatusColor = Status.rfind("SORTED", 0) == 0 ? "\033[1;32m" : Status.rfind("ERROR", 0) == 0 ? "\033[1;31m" : "\033[1;33m";
	Out << "  " << StatusColor << Status << "\033[0m\033[K\n";
	Out << "\033[K\n";
	for (int Row = Height; Row >= 1; Row--) {
		Out << "  ";
		for (int i = 0; i < N; i++) {
			int H = (A[i] * Height + Top - 1) / Top;
			if (H < 1) H = 1;
			if (H >= Row) {
				Out << (IsHot(i) ? HotColor(HotKind) : BarColor(A[i], Top)) << string(Width, ' ') << "\033[0m";
			} else {
				Out << string(Width, ' ');
			}
			if (i + 1 < N) Out << ' ';
		}
		Out << "\033[K\n";
	}
	Out << "  \033[90m";
	for (int i = 0; i < N; i++) {
		string Label = to_string(i + 1);
		int Left = (Width - static_cast<int>(Label.size())) / 2;
		if (Left < 0) Left = 0;
		int Right = Width - Left - static_cast<int>(Label.size());
		if (Right < 0) Right = 0;
		Out << string(Left, ' ') << Label << string(Right, ' ');
		if (i + 1 < N) Out << ' ';
	}
	Out << "\033[0m\033[K\n\033[J";
	cout << Out.str() << flush;
}
void Show(const string &Msg, const vector<int> &Hots, int Kind) {
	Status = Msg;
	Hot = Hots;
	HotKind = Kind;
	Draw();
	if (DelayMs > 0) this_thread::sleep_for(chrono::milliseconds(DelayMs));
}
string IndexText(int Index) {
	return "A[" + to_string(Index + 1) + "]";
}
bool CompareA(int First, int Second, bool WantLess) {
	int X = A[First], Y = A[Second];
	Cmp++;
	bool Result = WantLess ? X < Y : X > Y;
	string Relation = WantLess ? " < " : " > ";
	Show("compare " + IndexText(First) + "=" + to_string(X) + Relation + IndexText(Second) + "=" + to_string(Y), {First, Second}, 1);
	return Result;
}
void SwapA(int First, int Second) {
	int X = A[First], Y = A[Second];
	A[First] = Y;
	A[Second] = X;
	Mov += 3;
	Show("swap " + IndexText(First) + "=" + to_string(X) + " with " + IndexText(Second) + "=" + to_string(Y), {First, Second}, 2);
}
void SetA(int Index, int Value, const string &Msg) {
	A[Index] = Value;
	Mov++;
	Show(Msg, {Index}, 3);
}
void BubbleSort() {
	for (int i = 0; i < N - 1; i++) {
		bool Changed = false;
		for (int j = 0; j < N - 1 - i; j++) {
			if (CompareA(j, j + 1, false)) {
				SwapA(j, j + 1);
				Changed = true;
			}
		}
		if (!Changed) break;
	}
}
void SelectionSort() {
	for (int i = 0; i < N - 1; i++) {
		int Best = i;
		for (int j = i + 1; j < N; j++) {
			if (CompareA(j, Best, true)) Best = j;
		}
		if (Best != i) SwapA(i, Best);
	}
}
void InsertionSort() {
	for (int i = 1; i < N; i++) {
		int Key = A[i];
		Show("pick key = " + to_string(Key), {i}, 4);
		int j = i - 1;
		while (j >= 0) {
			int X = A[j];
			Cmp++;
			bool Greater = X > Key;
			Show("compare " + IndexText(j) + "=" + to_string(X) + " > key " + to_string(Key), {j}, 1);
			if (!Greater) break;
			SetA(j + 1, X, "shift " + IndexText(j) + " right");
			j--;
		}
		if (j + 1 != i) SetA(j + 1, Key, "place key " + to_string(Key));
	}
}
void ShellSort() {
	for (int Gap = N / 2; Gap > 0; Gap /= 2) {
		Show("shell gap = " + to_string(Gap), {}, 0);
		for (int i = Gap; i < N; i++) {
			int Key = A[i];
			int j = i;
			while (j >= Gap) {
				int X = A[j - Gap];
				Cmp++;
				bool Greater = X > Key;
				Show("gap " + to_string(Gap) + ": compare " + IndexText(j - Gap) + "=" + to_string(X) + " > key " + to_string(Key), {j - Gap}, 1);
				if (!Greater) break;
				SetA(j, X, "gap " + to_string(Gap) + ": shift " + IndexText(j - Gap) + " right");
				j -= Gap;
			}
			if (j != i) SetA(j, Key, "place key " + to_string(Key));
		}
	}
}
void MergeRange(int Left, int Mid, int Right) {
	vector<int> L(A.begin() + Left, A.begin() + Mid + 1);
	vector<int> R(A.begin() + Mid + 1, A.begin() + Right + 1);
	int i = 0, j = 0, k = Left;
	while (i < static_cast<int>(L.size()) && j < static_cast<int>(R.size())) {
		int X = L[i], Y = R[j];
		Cmp++;
		bool TakeLeft = X <= Y;
		if (TakeLeft) i++;
		else j++;
		A[k] = TakeLeft ? X : Y;
		Mov++;
		Show("merge " + to_string(X) + (TakeLeft ? " <= " : " > ") + to_string(Y) + ", write " + IndexText(k), {k}, 3);
		k++;
	}
	while (i < static_cast<int>(L.size())) {
		A[k] = L[i++];
		Mov++;
		Show("merge: write left " + to_string(A[k]) + " to " + IndexText(k), {k}, 3);
		k++;
	}
	while (j < static_cast<int>(R.size())) {
		A[k] = R[j++];
		Mov++;
		Show("merge: write right " + to_string(A[k]) + " to " + IndexText(k), {k}, 3);
		k++;
	}
}
void MergeSort(int Left, int Right) {
	if (Left >= Right) return;
	int Mid = (Left + Right) / 2;
	MergeSort(Left, Mid);
	MergeSort(Mid + 1, Right);
	MergeRange(Left, Mid, Right);
}
void QuickSort(int Left, int Right) {
	if (Left >= Right) return;
	int Pivot = A[Right];
	Show("pivot " + IndexText(Right) + "=" + to_string(Pivot), {Right}, 4);
	int Store = Left;
	for (int j = Left; j < Right; j++) {
		if (CompareA(j, Right, true)) {
			if (Store != j) SwapA(Store, j);
			Store++;
		}
	}
	if (Store != Right) SwapA(Store, Right);
	QuickSort(Left, Store - 1);
	QuickSort(Store + 1, Right);
}
void SiftDown(int Index, int Size) {
	while (true) {
		int L = Index * 2 + 1, R = L + 1, Big = Index;
		if (L < Size && CompareA(L, Big, false)) Big = L;
		if (R < Size && CompareA(R, Big, false)) Big = R;
		if (Big == Index) break;
		SwapA(Index, Big);
		Index = Big;
	}
}
void HeapSort() {
	for (int i = N / 2 - 1; i >= 0; i--) SiftDown(i, N);
	for (int End = N - 1; End > 0; End--) {
		SwapA(0, End);
		SiftDown(0, End);
	}
}
void CountingSort() {
	int Low = *min_element(A.begin(), A.end());
	int High = *max_element(A.begin(), A.end());
	vector<int> Count(High - Low + 1, 0);
	for (int i = 0; i < N; i++) {
		int Bucket = A[i] - Low;
		Count[Bucket]++;
		Show("count " + IndexText(i) + "=" + to_string(A[i]) + ", bucket " + to_string(A[i]) + " -> " + to_string(Count[Bucket]), {i}, 4);
	}
	int Pos = 0;
	for (int Value = Low; Value <= High; Value++) {
		int Amount = Count[Value - Low];
		for (int c = 0; c < Amount; c++) {
			A[Pos] = Value;
			Mov++;
			Show("write " + to_string(Value) + " to " + IndexText(Pos), {Pos}, 3);
			Pos++;
		}
	}
}
void RadixSort() {
	int High = *max_element(A.begin(), A.end());
	long long Exp = 1;
	int Pass = 0;
	while (High / Exp > 0) {
		Pass++;
		vector<int> Out(N, 0), Count(10, 0);
		Show("radix pass " + to_string(Pass) + ", digit place " + to_string(Exp), {}, 0);
		for (int i = 0; i < N; i++) {
			int Digit = (A[i] / Exp) % 10;
			Count[Digit]++;
			Show("pass " + to_string(Pass) + ": " + IndexText(i) + "=" + to_string(A[i]) + " digit " + to_string(Digit), {i}, 4);
		}
		for (int d = 1; d < 10; d++) Count[d] += Count[d - 1];
		for (int i = N - 1; i >= 0; i--) {
			int Digit = (A[i] / Exp) % 10;
			Out[--Count[Digit]] = A[i];
			Mov++;
			Show("pass " + to_string(Pass) + ": place " + to_string(A[i]) + " from " + IndexText(i), {}, 3);
		}
		for (int i = 0; i < N; i++) {
			A[i] = Out[i];
			Mov++;
			Show("pass " + to_string(Pass) + ": copy back " + IndexText(i) + "=" + to_string(A[i]), {i}, 3);
		}
		Exp *= 10;
	}
}
void SortById(int Id) {
	if (Id == 1) BubbleSort();
	else if (Id == 2) SelectionSort();
	else if (Id == 3) InsertionSort();
	else if (Id == 4) ShellSort();
	else if (Id == 5) MergeSort(0, N - 1);
	else if (Id == 6) QuickSort(0, N - 1);
	else if (Id == 7) HeapSort();
	else if (Id == 8) CountingSort();
	else if (Id == 9) RadixSort();
}
bool Verify(const vector<int> &Original) {
	if (!is_sorted(A.begin(), A.end())) return false;
	vector<int> Expected = Original;
	sort(Expected.begin(), Expected.end());
	return A == Expected;
}
void WaitEnter(const string &Msg) {
	cout << "  " << Msg << flush;
	string Dummy;
	getline(cin, Dummy);
}
bool ExecuteSort(int Id, const vector<int> &Original) {
	AlgoName = Names[Id];
	Cmp = 0;
	Mov = 0;
	Hot.clear();
	HotKind = 0;
	Status = "sorting";
	cout << "\033[?25l\033[2J\033[H";
	Draw();
	SortById(Id);
	bool Ok = Verify(Original);
	Hot.clear();
	HotKind = 0;
	Status = Ok ? "SORTED" : "ERROR";
	Draw();
	cout << "\033[?25h\033[0m" << flush;
	return Ok;
}
void RunOne(int Id) {
	vector<int> Original = A;
	ExecuteSort(Id, Original);
	WaitEnter("Press Enter to return to the menu...");
	A = Original;
}
void RunAll() {
	vector<int> Original = A;
	for (int Id = 1; Id <= 9; Id++) {
		A = Original;
		ExecuteSort(Id, Original);
		WaitEnter(Id < 9 ? "Press Enter for the next algorithm..." : "Press Enter to return to the menu...");
		if (!cin) break;
	}
	A = Original;
}
void RandomData() {
	A.assign(N, 0);
	for (int i = 0; i < N; i++) A[i] = RandomInt(1, 99);
}
int Ask(const string &Prompt, int Low, int High, int Default) {
	while (true) {
		cout << Prompt << flush;
		string Input;
		if (!getline(cin, Input)) return Default;
		if (Input.empty()) return Default;
		stringstream Stream(Input);
		int Value;
		char Extra;
		if ((Stream >> Value) && !(Stream >> Extra) && Value >= Low && Value <= High) return Value;
		cout << "  expected a number in " << Low << ".." << High << "\n";
	}
}
void InputData() {
	cout << "\033[2J\033[H  \033[1;97mMANUAL INPUT\033[0m\n\n";
	int Amount = Ask("  how many values (5-32): ", 5, 32, N);
	vector<int> Data(Amount, 0);
	for (int i = 0; i < Amount; i++) Data[i] = Ask("  value " + to_string(i + 1) + " (1-999): ", 1, 999, RandomInt(1, 99));
	A = Data;
	N = Amount;
}
void Menu() {
	cout << "\033[2J\033[H";
	cout << "  \033[1;97mSORT VISUALIZER\033[0m   \033[90mn " << N << "   speed " << Speed << "/9\033[0m\n\n";
	cout << "  \033[1;36m[1]\033[0m Bubble       \033[1;36m[2]\033[0m Selection    \033[1;36m[3]\033[0m Insertion\n";
	cout << "  \033[1;36m[4]\033[0m Shell        \033[1;36m[5]\033[0m Merge        \033[1;36m[6]\033[0m Quick\n";
	cout << "  \033[1;36m[7]\033[0m Heap         \033[1;36m[8]\033[0m Counting     \033[1;36m[9]\033[0m Radix\n\n";
	cout << "  \033[1;36m[A]\033[0m All   \033[1;36m[R]\033[0m Random   \033[1;36m[M]\033[0m Manual   \033[1;36m[N]\033[0m Size   \033[1;36m[S]\033[0m Speed   \033[1;36m[Q]\033[0m Quit\n";
	cout << "\n  choice: " << flush;
}
int main() {
	ios::sync_with_stdio(false);
	EnableAnsi();
	cout << "\033[?1049h\033[2J\033[H";
	RandomData();
	DelayMs = SpeedDelay[Speed];
	while (true) {
		Menu();
		string Input;
		if (!getline(cin, Input)) break;
		size_t Start = 0;
		while (Start < Input.size() && isspace(static_cast<unsigned char>(Input[Start]))) Start++;
		if (Start == Input.size()) continue;
		string Choice = Input.substr(Start);
		char C = static_cast<char>(tolower(static_cast<unsigned char>(Choice[0])));
		if (C == 'q') break;
		if (C >= '1' && C <= '9' && Choice.size() == 1) {
			RunOne(C - '0');
		} else if (C == 'a') {
			RunAll();
		} else if (C == 'r') {
			RandomData();
		} else if (C == 'm') {
			InputData();
		} else if (C == 'n') {
			cout << "\033[2J\033[H  \033[1;97mSET SIZE\033[0m\n\n";
			N = Ask("  size (5-32): ", 5, 32, N);
			RandomData();
		} else if (C == 's') {
			cout << "\033[2J\033[H  \033[1;97mSET SPEED\033[0m\n\n";
			Speed = Ask("  speed 1 (slow) .. 9 (fast): ", 1, 9, Speed);
			DelayMs = SpeedDelay[Speed];
		} else {
			cout << "\n  unknown choice\n";
			WaitEnter("Press Enter to continue...");
		}
	}
	RestoreTerminal();
	return 0;
}
