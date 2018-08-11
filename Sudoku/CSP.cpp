extern "C" {
	#include "CSP.h"
	bool	Solve(){
		if (CheckSyntax()){
			Initial_Limitation();
			return	Solve_Sudoku();
		}
		return	0;
	}

	int		Solve_Sudoku(){
		bool	TLimitation[9][9][10] = { 0 };
		int		i = MRVI;
		int		j = MRVJ;
		if (Check_Compliated())
			return	1;
		MRV();
		for (int k = 1; k <= 9; k++)
			if (Limitation[MRVI][MRVJ][k] == 0){
				for (int m = 0; m < 9; m++)
					for (int n = 0; n < 9; n++)
						for (int o = 0; o <= 9; o++)
							TLimitation[m][n][o] = Limitation[m][n][o];

				ForwardChecking(k);
				Sudoku_Values[MRVI][MRVJ] = k;
				if (Solve_Sudoku())
					return	1;
				for (int m = 0; m < 9; m++)
					for (int n = 0; n < 9; n++)
						for (int o = 0; o <= 9; o++)
							Limitation[m][n][o] = TLimitation[m][n][o];
			}
		MRVI = i;
		MRVJ = j;
		Sudoku_Values[MRVI][MRVJ] = 0;
		return	0;
	}

	bool	Check_Compliated(){
		bool	f = false;
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (Sudoku_Values[i][j] == 0){
					f = true;
					i = 9;
					j = 9;
					break;
				}
		return	!f;
	}

	void	ForwardChecking(int	v){
		int	x = MRVI / 3;
		int	y = MRVJ / 3;
		for (int i = 0; i < 9; i++)
			if (i != MRVI)
				Limitation[i][MRVJ][v] = true;
		for (int j = 0; j < 9; j++)
			if (j != MRVJ)
				Limitation[MRVI][j][v] = true;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				if (i != MRVI && j != MRVJ)
					Limitation[i + x * 3][j + y * 3][v] = true;
	}


	int	CheckSyntax(){
		// i -> Rows
		// j -> Columns
		char	t[64] = { 0 };
		memset(Limitation, 0, 81 * 10 * sizeof(bool));
		for (int i = 0; i < 9; i++){
			memset(TempLimitation, 0, 90 * sizeof(bool));
			for (int j = 0; j < 9; j++){
				if (Sudoku_Values[i][j] == 0)
					for (int k = 0; k < 9; k++){
						if (k != j && Sudoku_Values[i][k]){
							if (TempLimitation[j][Sudoku_Values[i][k]]){
								sprintf(t, "At row %i duplicate value(%i) occured!", i, Sudoku_Values[i][k]);
								MessageBox(0, t, "Error!", MB_ICONINFORMATION);
								return 0;
							}
							else
								TempLimitation[j][Sudoku_Values[i][k]] = true;
						}
					}
			}
		}
		for (int j = 0; j < 9; j++){
			memset(TempLimitation, 0, 90 * sizeof(bool));
			for (int i = 0; i < 9; i++){
				if (Sudoku_Values[i][j] == 0)
					for (int k = 0; k < 9; k++){
						if (k != i && Sudoku_Values[k][j]){
							if (TempLimitation[i][Sudoku_Values[k][j]]){
								sprintf(t, "At column %i duplicate value(%i) occured!%i,%i", i, j, k, Sudoku_Values[k][j]);
								MessageBox(0, t, "Error!", MB_ICONINFORMATION);
								return 0;
							}
							else
								TempLimitation[i][Sudoku_Values[k][j]] = true;
						}
					}
			}
		}
		for (int x = 0; x < 3; x++)
			for (int y = 0; y < 3; y++){
				memset(TempLimitation, 0, 90 * sizeof(bool));
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++){
						for (int m = 0; m < 3; m++)
							for (int n = 0; n < 3; n++){
								if (m != i && n != j && Sudoku_Values[m + x * 3][n + y * 3]){
									if (TempLimitation[i * 3 + j][Sudoku_Values[m + x * 3][n + y * 3]]){
										sprintf(t, "At Squar(%i,%i) duplicate value : %i", x, y, Sudoku_Values[m + x * 3][n + y * 3]);
										MessageBox(0, t, "Error!", MB_ICONINFORMATION);
										return 0;
									}
									else
										TempLimitation[i * 3 + j][Sudoku_Values[m + x * 3][n + y * 3]] = true;
								}
							}
					}
			}
		return	1;
	}




	void	Initial_Limitation(){
		// i -> Rows
		// j -> Columns
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (Sudoku_Values[i][j] == 0)
					for (int k = 0; k < 9; k++)
						if (k != j && Sudoku_Values[i][k])
							Limitation[i][j][Sudoku_Values[i][k]] = true;
		for (int j = 0; j < 9; j++)
			for (int i = 0; i < 9; i++)
				if (Sudoku_Values[i][j] == 0)
					for (int k = 0; k < 9; k++)
						if (k != i && Sudoku_Values[k][j])
							Limitation[i][j][Sudoku_Values[k][j]] = true;
		for (int x = 0; x < 3; x++)
			for (int y = 0; y < 3; y++)
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
						for (int m = 0; m < 3; m++)
							for (int n = 0; n < 3; n++)
								if (m != i && n != j && Sudoku_Values[m + x * 3][n + y * 3])
									Limitation[i + x * 3][j + y * 3][Sudoku_Values[m + x * 3][n + y * 3]] = true;
	}


	void		MRV(){
		int		limitiation, tlimitiation = -1;
		MRVI = 0;
		MRVJ = 0;
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++){
				limitiation = 0;
				for (int k = 1; k <= 9; k++){
					if (Limitation[i][j][k])
						limitiation++;
				}
				if (tlimitiation < limitiation && Sudoku_Values[i][j] == 0){
					tlimitiation = limitiation;
					MRVI = i;
					MRVJ = j;
				}
			}
	}
}