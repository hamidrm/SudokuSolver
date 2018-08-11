#include	<Windows.h>
#include	<stdio.h>
extern	int	Sudoku_Values[9][9];

void	MRV();
void	Initial_Limitation();
bool	Solve();
void	ForwardChecking(int	v);
int		Solve_Sudoku();
int		CheckSyntax();
bool	Check_Compliated();

int		MRVI,MRVJ;
bool	Limitation[9][9][10]={0};
bool	TempLimitation[9][10]={0};