#include "BigNumbers2.h"
uint8_t  excla = 0;
char     oper;


BigNumber set (std::vector<double> const &vec) {
	// Serial.printf("entree set: floors = %d\n", vec.size());
	// Serial.printf("entree set: mant = %f\n", vec[0]);
	// for (byte i = 1; i < vec.size(); i++) Serial.printf("entree set: expo %d = %f\n", i, vec[i]);

	std::vector<double> Vec;
	Vec.assign(vec.begin(),vec.end());
	/*  floors is the size of the vector
		for example 1.2 10^(34 10^5.6) has 3 floors : N[0] ... N[2]
		but it can be written as 7.550925 × 10^13535643 (2 floors)
				or preferably as 7.550925 × 10^1.3535643 x 10^7 (3 floors) */
	uint16_t floors = Vec.size();
	BigNumber N(floors, 0);

	if (floors == 1) { // no exponent
		if (Vec[0] >= 10.0f) {
			N.push_back(0);
			while (Vec[0] >= 10.0f) {
				++N[1];
				Vec[0] /= 10.0f;
			}
		}
		N[0] = (int64_t) (Vec[0] * maxPower);
		return N;

	} else if (floors == 2) { // aaa.aaaa * 10^bbbb.bbb
		int64_t integ = (int64_t) Vec[1];
		double frac = Vec[1] - integ;
		Vec[0] *= pow(10.0f, frac);
		while (Vec[0] >= 10.0f) {
			++Vec[1];
			Vec[0] /= 10.0f;
		}
		N[1] = Vec[1];
		N[0] = (int64_t) (Vec[0] * maxPower);
		if (N[1] >= 1e7) {
			double x = N[1];
			N.push_back(0);
			while (x >= 1e7) {
				++N[2];
				x /= 10.0f;
			}
			N[1] = x * maxPower;
		}
		return N;

	} else { // More than 2 floors
		double x = log10(Vec[floors - 2]) + Vec[floors - 1];
		if (x < MAXPREC) { // can reduce the last floor
			N.erase(N.end() - 1);
			x = pow(10.0f, x);
			N[floors - 2] = (int64_t) x;
			double frac = x - N[floors - 2];
			Vec[floors - 3] *= pow(10.0f, frac);
			while (Vec[floors - 3] >= 10.0f) {
				++N[floors - 2];
				Vec[floors - 3] /= 10.0f;
			}
			N[floors - 3] = (int64_t) (Vec[floors - 3] * maxPower);
			for (int i = floors - 4; i >= 0; i--) {
				while (Vec[i] >= 10.0f) Vec[i] /= 10.0f;
				N[i] = (int64_t) (Vec[i] * maxPower);
			}
			return N;

		} else { // can't reduce the last floor
			N[floors - 1] = (int64_t) Vec[floors - 1];
			double frac = Vec[floors - 1] - N[floors - 1];
			Vec[floors - 2] *= pow(10.0f, frac);
			while (Vec[floors - 2] >= 10.0f) {
				++N[floors - 1];
				Vec[floors - 2] /= 10.0f;
			}
			N[floors - 2] = (int64_t) (Vec[floors - 2] * maxPower);
			for (int i = floors - 3; i >= 0; i--) {
				while (Vec[i] >= 10) Vec[i] /= 10.0f;
				N[i] = (int64_t) (Vec[i] * maxPower);
			}


			if (N[floors - 1] >= 1e7) {
				N.push_back(0);
				double x = N[floors - 1];
				while (x >= 1e7) {
					x /= 10;
					++ N[floors];
				}
				N[floors - 1] = x * maxPower;
			}


			return N;
		}
	}
}

// Clean numbers with lots of 99999...
int64_t cleanMe(int64_t x) {
	char buff[25];
	int l = log10(x);
	sprintf(buff, "%lld", x);
	char * pch;
	pch = strstr(buff, "9999999");
	if (pch == NULL) return x;
	int position = pch - buff;
	int64_t y = (1 + x / (int64_t)(pow(10, l - position))) 
				* (int64_t)(pow(10, l - position));
	return y;
}

// Display a given floor
void XXdisp (int64_t y, uint8_t prec, bool point) {
	char buff[20];
	uint8_t nDec = prec;
	int64_t x = cleanMe(y);
	sprintf(buff, "%lld", x);
	Serial.print(buff[0]);
	if (point) Serial.print('.');
	else nDec = 20;
	for (uint8_t i = 0; i < nDec; i++) {
		if (buff[i + 1] == 0) return;
		Serial.print(buff[i + 1]);
	}
}

// Display the structure of a BigNumber
void displayStruct (BigNumber const & N) {
	Serial.printf("%d floors\n", N.size());
	Serial.print ("mant: ");
	XXdisp(N.at(0), 16, true);
	Serial.println();

	for (uint8_t i = 1; i < N.size(); i++) {
		Serial.printf("floor %d expo: ", i + 1);
		XXdisp(N.at(i), 12, (i + 1 != N.size()));
		Serial.println();
	}
}

// Display a bigNumber in a human understandable form (prec : number of figures)
void displayBigNumber (BigNumber const & N, uint8_t prec) {
	if (N.at(0) != pow(10,MAXPREC)) XXdisp (N.at(0), prec, true);
	for (uint8_t i = 1; i < N.size(); i++) {
		if (i + 1 == N.size()) {
			Serial.print(" 10^");
			XXdisp(N.at(i), prec, false);
		} else {
			Serial.print(" 10^(");
			if (N.at(i) != pow(10,MAXPREC)) XXdisp(N.at(i), prec, true);
		}
	}
	for (uint8_t i = 1; i < N.size() - 1; i++) Serial.print(")");
	Serial.print(" ");
}


BigNumber bigAdd(BigNumber & N1, BigNumber & N2) {
	// Serial.printf("add 1: floor = %d\n", N1.size());
	// for (byte i = 0; i < N1.size(); i++) Serial.printf("add 1: expo %d = %lld\n", i, N1[i]);
	// Serial.printf("add 2: floor = %d\n", N2.size());
	// for (byte i = 0; i < N2.size(); i++) Serial.printf("add 2: expo %d = %lld\n", i, N2[i]);

	uint8_t N1s = N1.size();
	if (N1s == 0) return N2;
	uint8_t N2s = N2.size();
	if (N2s == 0) return N1;
	uint8_t floors = max(N1s, N2s);
	BigNumber N(floors, 0);

	if (N1s < 3 && N2s < 3) { // perform real addition
		if (N1s + N2s == 2) { // a + b
			N[0] = N1[0] + N2[0];
			if (N[0] >= (int64_t)(10 * maxNumber)) {
				N.push_back(0);
				while (N[0] >= 10 * maxNumber) {
					++N[1];
					N[0] /= 10;
				}
			}
			return N;
		} else if (N1s == 2 && N2s == 1) { // a 10^b + c
			if (N1[1] > MAXPREC) return N1;
			N[0] = N1[0] + N2[0] * pow(10, MAXPREC - N1[1]);
			N[1] = N1[1];
			return N;
		} else if (N1s == 1 && N2s == 2) { // a + b 10^c
			if (N2[1] > MAXPREC) return N2;
			N[0] = N2[0] + N1[0] * pow(10, MAXPREC - N2[1]);
			N[1] = N2[1];
			return N;
		}  else { // a 10^b + c 10^d
			int delta = N2[1] - N1[1];
			if (delta >  MAXPREC) return N2;
			if (delta < -MAXPREC) return N1;
			if (delta >= 0) {
				N[0] = N2[0] + N1[0] / pow(10, delta);
				N[1] = N2[1];
				while (N[0] >= 10 * maxNumber) {
					++N[1];
					N[0] /= 10;
				}
				return N;
			} else {
				N[0] = N1[0] + N2[0] / pow(10, -delta);
				N[1] = N1[1];
				while (N[0] >= 10 * maxNumber) {
					++N[1];
					N[0] /= 10;
				}
				return N;
			}
		}
	}
	// Case of very big numbers
	if (N1s != N2s) return (N1s > N2s)? N1 : N2; // return biggest number

	// Case of same exponents
	BigNumber N1e, N2e;
	N1e.assign(N1.begin(), N1.end() - 1);
	N2e.assign(N2.begin(), N2.end() - 1);
	if (isEqual(N1e, N2e)) {
		N1[0] += N2[0];
		if (N1[0] > 10 * maxNumber) N1[0] = maxNumber;
		return N1;
	}
	// Case of same number of exponents
	return (lessThan(N1, N2))? N2 : N1;
}

bool lessThan (BigNumber const & N1, BigNumber const & N2) {
	uint8_t N1s = N1.size();
	uint8_t N2s = N2.size();

	if (N1s < N2s) return true;
	if (N2s < N1s) return false;

	for (uint8_t i = N1s - 1; i >= 0; i--) {
		if (N1[i] < N2[i]) return true;
		if (N2[i] < N1[i]) return false;
	}
	return false;
}

bool isEqual (BigNumber const & N1, BigNumber const & N2) {
	uint8_t N1s = N1.size();
	uint8_t N2s = N2.size();

	if (N1s != N2s) return false;
	for (uint8_t i = N1s - 1; i >= 0; i--) if (N1[i] != N2[i]) return false;
	return true;
}


bool lessEqual (BigNumber const & N1, BigNumber const & N2) {
	return (lessThan(N1, N2) || isEqual(N1, N2));
}

bool greaterThan (BigNumber const & N1, BigNumber const & N2) {
	return !lessEqual(N1, N2);
}

bool greaterEqual (BigNumber const & N1, BigNumber const & N2) {
	return !lessThan(N1, N2);
}

BigNumber bigMult(BigNumber & N1, BigNumber & N2) {
	uint8_t N1s = N1.size();
	uint8_t N2s = N2.size();
	if (N1s != 1 && N1[N1s - 1] >= 10)  {
		uint8_t n = log10(N1[N1s - 1]);
		N1[N1s - 1] *= pow(10, MAXPREC - n);
		N1.push_back(n);
		++N1s;
	}
	if (N2s != 1 && N2[N2s - 1] >= 10)  {
		uint8_t n = log10(N2[N2s - 1]);
		N2[N2s - 1] *= pow(10, MAXPREC - n);
		N2.push_back(n);
		++N2s;
	}

	// Serial.printf("mult 1: floor = %d\n", N1.size());
	// for (byte i = 0; i < N1.size(); i++) Serial.printf("mult 1: expo %d = %lld\n", i, N1[i]);
	// Serial.printf("mult 2: floor = %d\n", N2.size());
	// for (byte i = 0; i < N2.size(); i++) Serial.printf("mult 2: expo %d = %lld\n", i, N2[i]);

	if (N1s + N2s == 2) { // case a * b
		int64_t mant = (N1[0] / pow(10, MAXPREC / 2)) * (N2[0] / pow(10, MAXPREC / 2));
		if (mant < (int64_t)maxNumber) {
			BigNumber N(1, 0);
			N[0] = mant;
			return N;
		} else {
			BigNumber N(2, 0);
			N[0] = mant;
			while (N[0] >= maxNumber) {
				N[0] /= 10;
				++N[1];
			}
			return N;
		}
	} else if (N1s + N2s == 3) {
		BigNumber N(2, 0);
		N[0] = (N1[0] / pow(10, MAXPREC / 2)) * (N2[0] / pow(10, MAXPREC / 2));
		if (N1s > N2s) N[1] = N1[1]; // (a * 10^b) * c
		else N[1] = N2[1];
		while (N[0] >= maxNumber) {
			N[0] /= 10;
			++N[1];
		}
		return N;
	}

	// real big numbers
	int64_t mant = (N1[0] / pow(10.0f, MAXPREC / 2)) * (N2[0] / pow(10.0f, MAXPREC / 2));
	BigNumber N1e(N1s - 1, 0), N2e(N2s - 1, 0), Np;
	for (uint8_t i = 1; i < N1s; i++) N1e[i - 1] = N1[i];
	for (uint8_t i = 1; i < N2s; i++) N2e[i - 1] = N2[i];
	Np = bigAdd(N1e, N2e);
	// Simplify Np
	uint8_t Nps = Np.size();
	if (Nps > 1 && Np[Nps - 1] < MAXPREC) {
		Np[Nps - 2] /= pow(10, MAXPREC - Np[Nps - 1]);
		Np.resize(Nps - 1);
	}
	BigNumber N(Np.size() + 1, 0);

	N[0] = mant;
	for (uint8_t i = 0; i < Np.size(); i++) N[i + 1] = Np[i];
	if (N.size() <= 2) {
		while (mant >= maxNumber) {
			mant /= 10.0f;
			++N[1];
		}		
	}
	// while (N[N.size() - 1] == 0) N.resize(N.size() - 1);

	return N;
}

// Compute A ^ B = 10 ^ (B * log10 (A))
BigNumber bigPower (BigNumber & A, BigNumber & B) {
	// Serial.printf("pow 1: floor = %d\n", A.size());
	// for (byte i = 0; i < A.size(); i++) Serial.printf("pow 1: expo %d = %lld\n", i, A[i]);
	// Serial.printf("pow 2: floor = %d\n", B.size());
	// for (byte i = 0; i < B.size(); i++) Serial.printf("pow 2: expo %d = %lld\n", i, B[i]);

	double mantA = log10((double)A.at(0) / maxPower); // log10 of mantissa of A
	uint8_t nA = A.size();
	BigNumber E(1, 0); // will contain log10(A)

	if (nA == 1) {
		E[0] = (int64_t)(mantA * maxPower);
	} else if (nA == 2) {
		E[0] = A[1] * maxPower;
		E[0] += (int64_t)(mantA * maxPower);
	} else {
		E.resize(nA - 1);
		for (uint8_t i = 1; i < nA; i++) E[i - 1] = A[i]; 
	}
	E = bigMult(B, E); 			 // B * log10 (A)
	BigNumber P(E.size() + 1, 0); // P contains the power : 10^(B * log10 (A))

	P[0] = maxPower;
	if (E.size() == 1) {
		int64_t integ = E[0] / maxPower;
		double frac = ((double)E[0] / maxPower) - integ;
		frac = pow(10.0f, frac);
		P[1] = integ;
		while (frac >= 10.0f) {
			++P[1];
			frac /= 10.0f;
		}
		P[0] = (int64_t) (frac * maxPower);
		return P;
	}

	for (uint8_t i = 0; i < E.size(); i++) P[i + 1] = E[i]; // 10 ^ (B * log10 (A))
	// Simplify P
	uint8_t sP = P.size();
	if (P[sP - 1] >= MAXPREC) return P;

	double V = (double)P[sP - 2] / maxPower * pow(10.0f, P[sP - 1]);
	int64_t integ = V;
	double frac = V - integ;
	frac = pow(10.0f, frac);
	P[sP - 2] = integ;
	while (frac >= 10.0f) {
		++P[sP - 1];
		frac /= 10.0f;
	}
	P[sP - 3] = (int64_t) (frac * maxPower);
	P.resize(sP - 1);
	return P;
}

// Compute log_10 (x!)
BigNumber bigStirling (BigNumber & N) {
	// Serial.printf("fact: floors = %d\n", N.size());
	// for (byte i = 0; i < N.size(); i++) Serial.printf("expo %d = %lld\n", i, N[i]);

	if (N.size() == 1) { 		// Compute factorial
		double fact = 1.0f;
		int n = (int)(N[0] / maxPower);
		for (int i = 2; i <= n; i++) fact *= i;
		std::vector<double> Vec(1,0);
		Vec[0] = fact;
		BigNumber BigF = set(Vec);
		return BigF;
	} else if (N.size() == 2) { // Compute Stirling's approximation
		if (N[1] < MAXPREC) {
			// Serial.println("cas 0");
			BigNumber BigF(2, 0);
			double x = N[0] / pow(10.0f, MAXPREC - N[1]);
			double fact = x * log(x) - x;
			double y = 8 * x * x * x + 4 * x * x + x + .033333333333333333;
			fact += log(y) / 6.0 + 0.5723649429247;  // log_e(PI)/2
			double log_10 = 2.3025850929940457;      // log_e(10)
			y = fact / log_10;

			// put it in a bigNumber
			uint64_t intPart = y;
			double fracPart = y - intPart;
			double mant = pow(10.0, fracPart);
			BigF[1] = intPart;
			BigF[0] = mant * maxPower;
			while (BigF[0] > maxNumber) {
				BigF[0] /= 10;
				++BigF[1];
			}
			return BigF;
		} else { // Approximate N! by (N/e)^N = 10^(N log10 N - N log10 e)
			BigNumber M(N.size() - 1, 0);
			for (uint8_t i = 0; i < M.size(); i++) M[i] = N[i + 1]; // log10 N
			if (M.size() == 1) {
				M.push_back(0);
				uint64_t x = M[0];
				while (x > 10) {
					x /= 10;
					++M[1];
				}
				M[0] *= pow(10, MAXPREC - M[1]);
				M[0] += log10(N[0] / maxPower) * pow(10, MAXPREC - M[1]);
				M[0] -= (uint64_t)(pow(10, MAXPREC - M[1]) * 0.434294481903251f);
			}
			M = bigMult(M, N); // N log10 N
		// for (uint8_t i = 0; i < M.size(); i++) Serial.printf("M %d %lld\n", i,M[i]);
		// for (uint8_t i = 0; i < N.size(); i++) Serial.printf("N %d %lld\n", i,N[i]);
			BigNumber BigF(M.size() + 1, 0);
			for (uint8_t i = 0; i < M.size(); i++) BigF[i + 1] = M[i]; // 10^M
			BigF[0] = maxPower;
			return BigF;
		}
	}
	// Approximate N! by N^N
	BigNumber M(N.size() - 1, 0);
	for (uint8_t i = 0; i < M.size(); i++) M[i] = N[i + 1]; // log10 N
	M = bigMult(M, N); // N log10 N
	BigNumber BigF(M.size() + 1, 0);
	for (uint8_t i = 0; i < M.size(); i++) BigF[i + 1] = M[i]; // 10^M
	BigF[0] = maxPower;
	return BigF;
}

void splash() {
Serial.println("  ____  _         _   _                 _                   ");
Serial.println(" |  _ \\(_)       | \\ | |               | |                  ");
Serial.println(" | |_) |_  __ _  |  \\| |_   _ _ __ ___ | |__   ___ _ __ ___ ");
Serial.println(" |  _ <| |/ _` | | . ` | | | | '_ ` _ \\| '_ \\ / _ \\ '__/ __|");
Serial.println(" | |_) | | (_| | | |\\  | |_| | | | | | | |_) |  __/ |  \\__ \\");
Serial.println(" |____/|_|\\__, | |_| \\_|\\__,_|_| |_| |_|_.__/ \\___|_|  |___/");
Serial.println("           __/ |    _           _                     _   ");
Serial.println("          |___/    | |__ _  _  | |   ___ ___ ___ _ __| |_ ");
Serial.println("                   | '_ \\ || | | |__/ -_|_-</ -_) '_ \\  _|");
Serial.println("                   |_.__/\\_, | |____\\___/__/\\___| .__/\\__|");
Serial.println("                         |__/                   |_|       ");
}


// Display help info
void help() {
  Serial.println("--------------------------------------------------------------------");
  Serial.println("User's manual:");
  Serial.println("A big number is a set of numbers separated by 'e' or 'E'");
  Serial.println("As in engineering format, the 'e' stands for 10^");
  Serial.println("Several consecutive 'e' stand for 10^10^... 10^");
  Serial.println("Enter 2 big numbers and operators (+ * ^) or comparators (< > =)");
  Serial.println("Use ! for the factorial. !! is the factorial of the factorial, etc.");
  Serial.println("Use k for the Knuth exponentiation");
  Serial.println("Some examples:");
  Serial.println("12.345e67!    : factorial of 12.345 10^(67)");
  Serial.println("12e3.4e56 !!  : factorial of factorial of 12 10^(3.4 10^56)");
  Serial.println("3eee23 !      : factorial of 3.10^10^10^23");
  Serial.println("3aa4          : 3 up up 4 (Knuth notation, a = arrow)");
  Serial.println("2e3e4 + 3e5e7 : add the 2 big numbers");
  Serial.println("2e3e4 * 3e5e7 : multiply the 2 big numbers");
  Serial.println("2e3e4 ^ 3e5e7 : compute the power of the 2 big numbers (also p or P)");
  Serial.println("2e3e4 < 3e5e7 : compare the 2 big numbers");
  Serial.println("--------------------------------------------------------------------");
}

// read and store keyboard input
char* readSerial () {
  static char input[MAXINPUT];
  uint8_t index = 0;
  while (1) {
    if (Serial.available() > 0) {
      input[index] = Serial.read();
      if (input[index] == '\n' || index == MAXINPUT - 1) break;
      ++index;
    }
  }
  while (Serial.available()) Serial.read(); // clear input buffer
  input[index] = 0;
  return input;
}

bool parseOperator (char* input, bool message) {
  // search for an operator (><+*=!)
  uint8_t nbOp = 0;
  int index = 0;
  bool stop = false;
  bool found = false;
  while (input[index] != 0 && !stop) {
    switch (input[index]) {
      case '!':
        ++ excla;
        oper = input[index];
        found = true;
        break;
      case '<':
      case '>':
      case '=':
      case '+':
      case '*':
      case 'P':
      case 'p':
      case 'k':
      case 'K':
      case '^':
        oper = input[index];
        found = true;
        break;
      default:
        if (found) stop = true;
        break;
    }
  ++index;
  }

  if (!found && message) Serial.println ("found no operator!"); 
  // else {
  //   if (excla !=0) Serial.printf(" (%d)", excla);
  //   Serial.println();
  // }

  // Remove parsed characters
  if (!excla) --index;
  uint8_t index0 = index;
  while (input[index] != 0) input[index - index0] = input[index++];
  input[index - index0] = 0;
  return found;
}

// Parse the input string to find the requested operation
void parseInput(char* input, int prec) {
  BigNumber N1, N2, N;
  cleanInput(input);
  if (strchr(input,'h') != NULL || strchr(input,'?') != NULL) {help();  return;}

  Serial.printf("> %s\n", input);

  // Case of power tower:
  int nn = 0;
  int i = 0;
  while (input[i] != 0) {
	if (input[i] == '^' || input[i] == 'p' || input[i] == 'P') ++nn;
	++i;
  }
  if (nn > 1) { parsePowerTower (input); return; }

  // Case of Knuth arrow notation
  nn = 0;
  i = 0;
  while (input[i] != 0) {
	if (input[i] == 'a' || input[i] == 'A') ++nn;
	++i;
  }
  if (nn > 0) { parseKnuth (input); return; }

  // Search the first number
  N1 = parseNumber(input);
  Serial.print("  ");
  displayBigNumber(N1, prec);

  // Search for an operator
  excla = 0;
  bool operat = parseOperator (input,true);
  while (operat) { // found an operator

    if (oper == '!') {
      uint8_t i = 0;
        while (i++ < excla) Serial.print('!');
        Serial.print (" =");
    } else {
      // Search the second number
      N2 = parseNumber(input);
      if (oper == 'p' || oper == 'P') oper = '^';
      Serial.printf("%c ", oper);
      // else Serial.print ("power ");
      displayBigNumber(N2, prec);
    }
    Serial.println();

    // Compute operation
    switch (oper) {
      case '!': {
        uint8_t i = 0;
          while (i++ < excla) {
            N = bigStirling(N1);
            N1 = N;
          }
          displayBigNumber(N, prec);
          break;        
        }
      case '+':
        N = bigAdd(N1, N2);
        Serial.print("= ");
        displayBigNumber(N, prec);
        break;
      case '*':
        N = bigMult(N1, N2);
        Serial.print("= ");
        displayBigNumber(N, prec);
        break;
      case '^':
        N = bigPower(N1, N2);
        Serial.print("= ");
        displayBigNumber(N, prec);
        break;
      case '=':
        Serial.print (isEqual(N1, N2)?"true":"false");
        break;
      case '>':
        Serial.print (greaterThan(N1, N2)?"true":"false");
        break;
      case '<':
        Serial.print (lessThan(N1, N2)?"true":"false");
        break;
      default:
        break;
    }
    N1 = N;
    operat = parseOperator (input, false);
  }
}

// Remove unwanted characters from the string
// Only keep: 0..9 e E ^ p P + = * ! < > .
void cleanInput(char* input) {
  char output[MAXINPUT];
  int i = 0;
  int N = 0;
  while (input[i] != 0) {
    // List of recognized characters:
    switch (input[i]) {
      case '0' ... '9': // Guess what...
      case 'e': // Exponent
      case 'E': // Exponent
      case '^': // Exponent
      case 'p': // power operator
      case 'P': // power operator
      case '<': // Comparison operator
      case '>': // Comparison operator
      case '=': // Comparison operator
      case '+': // Addition
      case '*': // Multiplication
      case '!': // Factorial
      case '.': // Decimal dot
      case 'a': // Knuth
      case 'A': // Knuth
      case 'h': // Display help
      case '?': // Display help
        output[N++] = input[i];
        break;
      default:
        break;
    }
    ++i;
  }
  output[N++] = '\0';
  memcpy (input, output, N);
}

// Compute N1 aa N2 ('a' stands for the up arrow of Knuth notation)
BigNumber tetration (BigNumber & N1, BigNumber & N2) {
	// Serial.printf("tetra 1: floor = %d\n", N1.size());
	// for (byte i = 0; i < N1.size(); i++) Serial.printf("tetra 1: expo %d = %lld\n", i, N1[i]);
	// Serial.printf("tetra 2: floor = %d\n", N2.size());
	// for (byte i = 0; i < N2.size(); i++) Serial.printf("tetra 2: expo %d = %lld\n", i, N2[i]);
	BigNumber A;
	A = N1;
	float e = N2[0] / pow(10,MAXPREC);
	if (N2.size() == 2) e *= pow(10,N2[1]);
	if (N2.size() > 2 || e > 250) {
		A = set({0});
		Serial.println ("Exponent too big: overflow");
		return A;
	}
	uint16_t expo = e;
	// Serial.printf("exponent= %d\n", expo);
	for (uint16_t i = 1; i < expo; i++) A = bigPower (N1, A);
	return A;
}

// Parse numbers in the case of Knuth arrow notation
BigNumber parseKnuth (char* input) {
	cleanInput(input);
	BigNumber A;
	A = set({0});
	char* k1 = strchr(input,'a');
	if (k1 == NULL) k1 = strchr(input,'A');
	if (k1 == NULL) { Serial.println ("No Knuth arrow found"); return A;}
	int pos = k1 - input;
	// Serial.println(pos);
	uint8_t nArrow = 1;
	while (input[pos + nArrow] == 'a' || input[pos + nArrow] == 'A') ++ nArrow;

	char number[MAXINPUT];
	memcpy (number, input, pos); // Not working ???
	// Serial.printf("**%s**\n",number);
	BigNumber N = parseNumber(number);
	displayBigNumber(N);
	// Serial.print (" ");
	for (uint8_t i = 0; i < nArrow; i++) Serial.print ("a");
	// uint8_t l = strlen(input);
	strcpy (number, k1 + nArrow);
	int e = atoi (number);
	Serial.printf(" %d = ",e);
	BigNumber E = set({e});

	// Compute Knuth exponentiation
	if (nArrow == 1) {
		A = bigPower (N, E);
	} else if (nArrow == 2) { // This is called Tetration
		A = tetration (N, E);
	} else if (nArrow == 3) { // This is called Pentation
		BigNumber M = set({100});
		if (greaterThan(N, M)) {
			displayBigNumber (N);
			Serial.println(" is too big !");
			return A;
		}
		A = N;
		for (uint8_t i = 1; i < e; i++)  A = tetration (N, A);
	} else {
		Serial.println ("Too many arrows !!!");
		return A;
	}
	if (A.size() < 11) displayBigNumber(A);
	else {
		Serial.println();
		displayStruct(A);
	}
	return A;
}

// Parse numbers in the case of power tower (such as "2.3e4^3^8.2^1e3")
void parsePowerTower (char* input) {
  cleanInput(input);
  // find end of power tower
  uint16_t maxIndex = 0;
  while (input[maxIndex] != 0 && ((input[maxIndex] >= '0' && input[maxIndex] <= '9')
    || input[maxIndex] == '.' || input[maxIndex] == 'e' || input[maxIndex] == 'E')
    || input[maxIndex] == '^') 
    ++maxIndex;

  // copy the power tower expression in a buffer
  char buff[MAXINPUT];
  char number[MAXINPUT];
  memcpy (buff, input, maxIndex);
  buff[maxIndex] = 0;

  BigNumber B, A, N;
  byte i = 0;

  // search the tower backwards from the end
  char * pch = strrchr(buff, '^');
  while (pch != NULL) {
    uint8_t position = pch - buff;
    int length = strlen(buff) - position;
    memcpy (number, pch + 1, length);
    number[length + 1] = 0;
    if (i == 0) N = parseNumber(number);
    else {
      B = N;
      A = parseNumber(number);
      N = bigPower(A, B);
    }
    ++i;
    buff[position] = 0;
    pch = strrchr(buff, '^');
  }
  B = N;
  A = parseNumber(buff);
  N = bigPower(A, B);
  displayBigNumber(N); Serial.println();
}

// Parse a big number from a char array
BigNumber parseNumber(char* input) {
	BigNumber N;
	uint16_t index = 0;
	// find end of number
	uint16_t maxIndex = 0;
	while (input[maxIndex] != 0 && ((input[maxIndex] >= '0' && input[maxIndex] <= '9')
		|| input[maxIndex] == '.' || input[maxIndex] == 'e' || input[maxIndex] == 'E')) 
		++maxIndex;

	// count the 'e's
	uint8_t nE = 0;
	while (input[index] != 0 && index < maxIndex) {
		if (input[index] == 'e' or input[index] == 'E') ++nE;
		++ index;
	}

	// begin parsing the big number
	double mant = 0.0f;
	double expo[MAXFLOORS] = {0};
	index = 0;
	int k = 0;
	uint8_t index0 = 0;
	char buff[MAXINPUT];

	// parse mantissa (if any)
	while ((input[index] >= '0' && input[index] <= '9') || input[index] == '.')
		buff[index - index0] = input[index++];
	buff[index - index0] = 0;
	mant = atof(buff);
	expo[k] = 0.0;

	for (int i = 0; i < nE; i++) {
		// erase 'e'
		while (input[index] == 'e' || input[index] == 'E') {
			if (input[index + 1] == 'e' || input[index + 1] == 'E') {
				// case of several consecutive 'e'
				expo[k] = 1.0;
				++k;
				--nE;
			}
			++index;
		}
		index0 = index;

		// parse exponent (if any)
		while ((input[index] >= '0' && input[index] <= '9') || input[index] == '.')
			buff[index - index0] = input[index++];
		buff[index - index0] = 0;
		expo[k] = atof(buff);
		++k;
		if (k > MAXFLOORS) {
			Serial.println("Too many floors !!!");
			return N;
		}
	}

  // Create big number form parsed data
	uint8_t floors = k;
	if (floors == 0) floors = 1;
	std::vector<double> Vec(floors + 1, 0);
	Vec[0] = mant;
	for (uint8_t i = 0; i < floors; i++) Vec[i + 1] = expo[i];
	N = set(Vec);
	// N = set(mant, expo, floors);

	// Remove parsed characters
	index0 = index;
	while (input[index] != 0) input[index - index0] = input[index++];
	input[index - index0] = 0;

	return N;
}