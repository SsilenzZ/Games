#include "Header.h"

void ShowRow(unsigned short int a)
{
	unsigned short int x = 0b1000000000000000, n;
	for (int i = 0; i < 16; i++)
	{
		n = a & x;
		if (n)
			std::cout << '1';
		else
			std::cout << '0';
		x = x >> 1;
	}
	std::cout << std::endl;
}

void ShowMas(unsigned short int * mas, short int size)
{
	system("cls");
	for (short int i = 0; i < size; i++)
	{
		ShowRow(mas[i]);
	}
}

short int countZeroLine(const Figure & figure, short int action)
{
	short int count = 0;
	short int help = 0;
	unsigned short int shift = 0b1000;
	switch (action)
	{
	case Up:
		for (int i = 0; i < 4; i++)
		{
			if (figure.Form[i])
				return count;
			else
				count++;
		}
		break;
	case Down:
		for (int i = 3; i >= 0; i--)
		{
			if (figure.Form[i])
				return count;
			else
				count++;
		}
		break;
	case Left:
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (figure.Form[j] & shift)
					break;
				else
					help++;
			}

			if (help == 4)
			{
				count++;
				help = 0;
				shift = shift >> 1;
			}
			else
				return count;
		}
		break;
	case Right:
		shift = 0b1;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (figure.Form[j] & shift)
					break;
				else
					help++;
			}

			if (help == 4)
			{
				count++;
				help = 0;
				shift = shift << 1;
			}
			else
				return count;
		}
		break;
	}

	return count;
}

void SetFigureForm(unsigned short int * mas, unsigned short int Value)
{
	unsigned short int shift = 0b1111000000000000;
	for (int i = 0; i < 4; i++)
	{
		mas[i] = (Value & shift) >> 12 - i * 4;
		shift = shift >> 4;
	}
}

void SetFigure(Figure & f, short int x, short int y, short int figure, short int state)
{
	if (figure == -1)
	{
		srand(time(NULL));
		figure = rand() % Line + 1;
	}

	if (state == -1)
	{
		if (figure == Square)
			state = 0;
		else
		{
			state = rand() % 4;
			if (figure >= ZLeft)
				state /= 2;
		}
	}

	f.figure = figure;
	f.state = state;
	f.x = x; f.y = y;
	switch (figure)
	{
	case Square:
		SetFigureForm(f.Form, SquareFirst);
		break;
	
	case GLeft:
		if (state == 0)
			SetFigureForm(f.Form, GLeftFirst);
		else if (state == 1)
			SetFigureForm(f.Form, GLeftSecond);
		else if (state == 2)
			SetFigureForm(f.Form, GLeftThird);
		else if (state == 3)
			SetFigureForm(f.Form, GLeftFourth);
		break;
	
	case GRight:
		if (state == 0)
			SetFigureForm(f.Form, GRightFirst);
		else if (state == 1)
			SetFigureForm(f.Form, GRightSecond);
		else if (state == 2)
			SetFigureForm(f.Form, GRightThird);
		else if (state == 3)
			SetFigureForm(f.Form, GRightFourth);
		break;
	
	case T:
		if (state == 0)
			SetFigureForm(f.Form, TFirst);
		else if (state == 1)
			SetFigureForm(f.Form, TSecond);
		else if (state == 2)
			SetFigureForm(f.Form, TThird);
		else if (state == 3)
			SetFigureForm(f.Form, TFourth);
		break;

	case Line:
		if (!state)
			SetFigureForm(f.Form, LineFirst);
		else
			SetFigureForm(f.Form, LineSecond);
		break;
	
	case ZLeft:
		if (!state)
			SetFigureForm(f.Form, ZLeftFirst);
		else
			SetFigureForm(f.Form, ZLeftSecond);
		break;
	
	case ZRight:
		if (!state)
			SetFigureForm(f.Form, ZRightFirst);
		else
			SetFigureForm(f.Form, ZRightSecond);
		break;
	
	}
}

void SetFigure(Figure & figure)
{
	SetFigure(figure, figure.x, figure.y, figure.figure, figure.state);
}

void ClearField(unsigned short int * field, short int size)
{
	for (int i = 0; i < size; i++)
		field[i] = 0;
}

bool InsertToField(Pole & pole, Figure & figure)
{
	if (CanIAct(pole, figure))
	{
		short int shift = pole.Xmax - figure.x - 3;
		short int start = figure.y;
		for (short int i = 0; (i < 4) && (start <= pole.Ymax); i++, start++)
		{
			if (shift >= 0)
				pole.field[start] = pole.field[start] | figure.Form[i] << shift;
			else
			{
				pole.field[start] = pole.field[start] | figure.Form[i] >> shift * -1;
			}
		}
		return true;
	}
	return false;
}

void DeleteFigure(Pole & pole, Figure & figure)
{
	short int shift = pole.Xmax - figure.x - 3;
	short int start = figure.y;
	for (short int i = 0; (i < 4) && (start <= pole.Ymax); i++, start++)
	{
		if (shift >= 0)
			pole.field[start] = pole.field[start] ^ figure.Form[i] << shift;
		else
		{
			pole.field[start] = pole.field[start] ^ figure.Form[i] >> shift * -1;
		}
	}
}

void NextState(Figure & figure)
{
	if (figure.figure == Square)
		return;
	if (figure.figure >= ZLeft)
	{
		if (!figure.state)
			figure.state = 1;
		else
			figure.state = 0;
	}
	else
	{
		if (figure.state == 3)
			figure.state = 0;
		else
			figure.state++;
	}

}

void PreviousState(Figure & figure)
{
	if (figure.figure == Square)
		return;
	if (figure.figure >= ZLeft)
	{
		if (!figure.state)
			figure.state = 1;
		else
			figure.state = 0;
	}
	else
	{
		if (figure.state == 0)
			figure.state = 3;
		else
			figure.state--;
	}

}

void NextFigure(Figure & figure, short int x, short int y, bool newF)
{
	if(newF)
		SetFigure(figure, x,y);
	else
	{
		figure.state = 0;
		figure.x = x; figure.y = y;
		if (figure.figure == Line)
			figure.figure = Square;
		else
			figure.figure++;

		SetFigure(figure, x, y,figure.figure,figure.state);
	}
	
}

bool CanIAct(const Pole & pole, const Figure & figure)
{
	short int shift = pole.Xmax - figure.x - 3;
	short int start = figure.y;
	for (short int i = 0; (i < 4) && (start <= pole.Ymax); i++, start++)
	{
		if (shift >= 0)
		{
			if (pole.field[start] & figure.Form[i] << shift)
				return false;
		}

		else
		{
			if (pole.field[start] & figure.Form[i] >> shift * -1)
				return false;
		}
	}
	return true;
}

bool CanIRotation(const Pole & pole, Figure & figure)
{
	if (CanIAct(pole, figure))
		return true;
	return false;
}

bool CanIMove(const Pole & pole, const Figure & figure, short int action)
{
	switch (action)
	{
		case Up:
			if (figure.y + countZeroLine(figure, action) < pole.Ymin)
				return false;
			break;
		case Down:
			if ((figure.y - countZeroLine(figure, action) + 3) > pole.Ymax)
				return false;
			break;
		case Left:
			if (figure.x + countZeroLine(figure, action) < pole.Xmin)
				return false;
			break;
		case Right:
			if ((figure.x - countZeroLine(figure, action) + 3) > pole.Xmax)
				return false;
			break;
	}
	if (CanIAct(pole, figure))
		return true;
	return false;
}

void Rotation(Pole & pole, Figure & figure)
{
	DeleteFigure(pole, figure);

	NextState(figure);
	SetFigure(figure);

	short int oldX = figure.x, oldY = figure.y;

	if (figure.x < pole.Xmin)
		figure.x = pole.Xmin - countZeroLine(figure, Left);
	if (figure.x > pole.Xmax - 3)
		figure.x = pole.Xmax + countZeroLine(figure, Right) - 3;
	if (figure.y < pole.Ymin)
		figure.y = pole.Ymin - countZeroLine(figure, Up);
	if (figure.y > pole.Ymax - 3)
		figure.y = pole.Ymax + countZeroLine(figure, Down) - 3;

	if (!CanIRotation(pole, figure))
	{
		PreviousState(figure);
		SetFigure(figure);
		figure.x = oldX; figure.y = oldY;
	}

	InsertToField(pole, figure);
}

bool move(Pole & pole, Figure & figure, short int action)
{
	bool result = true;
	DeleteFigure(pole, figure);
	
	switch (action)
	{
		case Up:
			figure.y--;
			if (!CanIMove(pole, figure, Up))
			{
				result = false;
				figure.y++;
			}
			break;
		case Down:
			figure.y++;
			if (!CanIMove(pole, figure, Down))
			{
				result = false;
				figure.y--;
			}
			break;
		case Left:
			figure.x--;
			if (!CanIMove(pole, figure, Left))
			{
				result = false;
				figure.x++;
			}
			break;
		case Right:
			figure.x++;
			if (!CanIMove(pole, figure, Right))
			{
				result = false;
				figure.x--;
			}
			break;
	}

	InsertToField(pole, figure);
	return result;
}

void DeleteLine(Pole & pole, short int y)
{
	pole.field[y] = 0;
	for (int i = y; i > pole.Ymin; i--)
	{
		pole.field[i] = pole.field[i - 1];
	}
}

void checkLines(Pole & pole, short int y, Game & game)
{
	int count = 0;
	for (int i = y + 3; i >= y; i--)
	{
		if (i > pole.Ymax)
			i = pole.Ymax;
		if (i < pole.Ymin)
			break;

		if (pole.field[i] == pole.checkNumber)
		{
			DeleteLine(pole, i);
			i++;
			count++;
		}
	}

	game.score += count * game.lengthLine;
	game.score += count * (game.level + 1);
	game.score += count * count * 5;
}