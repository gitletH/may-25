﻿//******************************************************//
//	tmath.cpp by Tianjiao Huang, 2016-2018				//
//******************************************************//


//Evaluate a mathematical expression
//Reference:https://www.youtube.com/watch?v=vq-nUF0G4fI&list=PL2_aWCzGMAwI3W_JlcBbtYTwiQSsOTa6P&annotation_id=annotation_1770609109&feature=iv&src_vid=MeRb_1bddWg
//

#include <iostream>
#include "tmath.h"
#include <stdlib.h>
#include "alglib/integration.h"


using namespace alglib;

std::string CurrentExpression;

const std::string M_PI = "3.1415926535897932";
const std::string M_E = "2.7182818284590452";
const std::list <std::string> operators
{
	"ln",		"log",
	"sin",		"cos",		"tan",
	"csc",		"sec",		"cot",
	"arcsin",	"arccos",	"arctan"
};

std::string parse(std::string exp)
{
	std::stack <std::string> buffer;
	std::string reserve;
	std::string bag;
	std::string sExpi;
	bool lastOneIsDigit = false;
	bool lastOneIsSymbol = false;

	exp.insert(0, 1, '(');				//put parentheses around the expression
	exp.push_back(')');

	PutTimesSimbolInFrontOfXAndOtherStuff(exp);


	//valid input? Try, throw, and catch



	for (unsigned int i = 0; i <= exp.length() - 1; i++)
	{
		sExpi.push_back(exp[i]);

		if (isNumber(exp[i]) || (exp[i] == '-' && (exp[i - 1] == '(' || exp[i - 1] == 'E' || exp[i - 1] == '^' || 
												   exp[i - 1] == '+' || exp[i - 1] == '-' || exp[i - 1] == '*' || exp[i - 1] == '/' )))//digit
		{
			reserve.push_back(exp[i]);
			for (i ++; std::isdigit(exp[i]) || exp[i] == '.'; i++)
			{
				reserve.push_back(exp[i]);
			}

			if (exp[i] == 'E')								//if 1E-10
			{
				reserve.push_back(exp[i]);
				reserve.push_back(exp[++i]);
				for (i++; std::isdigit(exp[i]) || exp[i] == '.'; i++)
				{
					reserve.push_back(exp[i]);
				}
			}

			reserve.push_back(' ');
			i -= 1;												//since the loop reach the end of the number, thus it need to move one step back

		}
		//else if (exp[i] == 'x')
		//{
		//	reserve.append(std::to_string(x));
		//	reserve.push_back(' ');
		//}
		else if (exp[i] == 'e')
		{
			reserve.append(M_E);
			reserve.push_back(' ');
		}
		else if (exp[i] == '(')//parenthese
		{
			buffer.push(sExpi);
		}
		else if (exp[i] == ')')
		{
			while (!buffer.empty())				//pop everything untill it reachs a left parenthese
			{
				if (buffer.top() == "(")
				{
					buffer.pop();
					break;
				}

				reserve.append(buffer.top());
				reserve.push_back(' ');
				buffer.pop();
			}
			try
			{
				if (!buffer.empty())				//if find an operator, pop it	
					if (isOperator(buffer.top())) //i.e. sin(1+2)
					{
						reserve.append(buffer.top());
						reserve.push_back(' ');
						buffer.pop();
					}
			}
			catch(...)
			{
				// Do nothing, since isOperator just checking is any trig left on the top of the
				std::cout << "Exception Thrown";
			}
		}
		else if (exp[i] == '+' || exp[i] == '-' || exp[i] == '*' || exp[i] == '/' || exp[i] == '^')
		{
			while ((!buffer.empty()) && hasHigherPrecedence(buffer.top(), sExpi))
			{
				reserve.append(buffer.top());
				reserve.push_back(' ');
				buffer.pop();
			}
			buffer.push(sExpi);
		}
		else if (exp[i] == 'p' && exp[i + 1] == 'i')
		{
			reserve.append(M_PI);
			reserve.push_back(' ');
			i++;
		}
		else						//take care of trigs and logs												
		{
			buffer.push(getOperator(exp, i));
			i += (int)buffer.top().size() - 1;
		}
				

		sExpi.clear();
	}
	if (!buffer.empty())
		tThrow("buffer is not empty");
	return reserve;
}

bool hasHigherPrecedence(std::string stack, std::string token)
{
	if (getOperLvl(stack) >= getOperLvl(token))
	{
		return true;
	}
	else
	{
		return false;
	}
}

unsigned int getOperLvl(std::string buffer)
{
	if (buffer == "(" || buffer ==  ")")
		return 0;
	else if (buffer == "+" || buffer ==  "-")
		return 2;
	else if (buffer == "*" || buffer ==  "/")
		return 3;
	else if (buffer == "^")
		return 4;
	else if (buffer ==	"ln"		|| buffer ==	"log"		|| buffer == 
						"sin"		|| buffer ==	"cos"		|| buffer == 		"tan"		|| buffer == 
						"csc"		|| buffer ==	"sec"		|| buffer == 		"cot"		|| buffer == 
						"arcsin"	|| buffer ==	"arccos"	|| buffer == 		"arctan"
			)
		return 1;
	else
		tThrow("invalid operator");
}

double evaluate(std::string exp, double x)
{
	double result;
	std::stack <double> container;
	std::string buffer;

	for (unsigned int i = 0; i <= exp.length() - 1; i++)
	{
		auto index = exp.find_first_of(' ', i);	//get each element

		if (index == i)		//reach the end of the expression
			continue;

		buffer = exp.substr(i, index - i);

		if (exp[i] == 'x')
		{
			result = x;
		}
		else if (isNumber(buffer[0])	|| (buffer[0] == '-' && buffer.size() > 1))	//Numbers
		{
				/*result = std::strtold(buffer.c_str(), NULL);*/
				result = std::stof(buffer);
		}
		else if(buffer.size() == 1)
		{
			double operL, operR;
			operR = container.top();
			container.pop();
			operL = container.top();
			container.pop();
			result = operate(operL, operR, buffer[0]);
		}
		else if (isOperator(buffer))
		{
			double operR = container.top();
			container.pop();
			result = operate(operR, buffer);
		}
		
		i = (int)index;
		container.push(result);
		buffer.erase();
	}
	return container.top();
}

double operate(double operL, double operR, char opert)
{
	switch (opert)
	{
	case '+':	return operL + operR;
		break;
	case '-':	return operL - operR;
		break;
	case '*':	return operL * operR;
		break;
	case '/':	return operL / operR;
		break;
	case '^':	return pow(operL, operR);
		break;
	default:
		tThrow("invalid operator");
		return 0;
	}
}

double operate(double operL, std::string opert) 
{
	if (opert == "sin")						return sin(operL);
	else if (opert == "cos")				return cos(operL);
	else if (opert == "tan")				return tan(operL);
	else if (opert == "csc")				return 1 / sin(operL);
	else if (opert == "sec")				return 1 / cos(operL);
	else if (opert == "cot")				return 1 / tan(operL);
	else if (opert == "arcsin")				return asin(operL);
	else if (opert == "arccos")				return acos(operL);
	else if (opert == "arctan")				return atan(operL);
	else if (opert == "ln")					return log(operL);
	else if (opert == "log")				return log10(operL);
	else
		tThrow("invalid oprator");
}

bool isNumber(char x)
{
	if (isdigit(x) || x == 'E' || x == '.' || x == 'x')
		return true;
	else
		return false;
}

bool isOperator(std::string x)
{
	for(auto s : operators)
	{
		if (x == s)
			return true;
	}
	return false;
}

std::string getOperator(std::string exp, unsigned int i)
{
	for (auto s : operators)
	{
		if (exp.find(s, i) == i)
		return s;
	}
	tThrow("invalid operator");
}

void int_function_1_func(double x, double xminusa, double bminusx, double &y, void *ptr)
{
	// this callback calculates f(x)=exp(x)
	y = evaluate(CurrentExpression, x);
}

double intergal(std::string exp, double a, double b, unsigned long n, intMethod method)
{
	switch (method)
	{
	case LeftRiemann:
	{
		double x = a;
		double dx = (b - a) / n;
		double result = 0;
		for (unsigned long long i = 0; i < n; i++)
		{
			result += evaluate(exp, x);
			x += dx;
		}
		result *= dx;
		return result;
	}
	break;

	case Simpson:
	{
		// Ensure that n can be divided by 2
		if (n % 2)
			n++;

		double x = a;
		double dx = (b - a) / n;
		double result = 0.0f;

		result += evaluate(exp, x);
		x += dx;

		for (unsigned long i = 1; i < n; i++)
		{
			// If i is odd
			if (i % 2)
			{
				result += 4.0f * evaluate(exp, x);
			}
			// If i is even
			else
			{
				result += 2.0f * evaluate(exp, x);
			}
			x += dx;
		}

		result += evaluate(exp, b);

		result *= dx / 3;
		return result;
	}
	break;

	case ALGLIB:
	{
		CurrentExpression = exp;
		alglib::autogkstate s;
		double v;
		alglib::autogkreport rep;

		alglib::autogksmooth(a, b, s);
		alglib::autogkintegrate(s, int_function_1_func);
		alglib::autogkresults(s, v, rep);

		return double(v);
	}
	break;

	default:
		break;
	}
	return 0.0f;
}

std::string PutTimesSimbolInFrontOfXAndOtherStuff(std::string& exp)
{
	for (unsigned int i = 0; i < exp.length(); i++)
	{
		if (exp[i + 1] == 'x' && isNumber(exp[i]))
		{
			exp.insert(i + 1, "*");
		}
		if (exp[i + 1] == '(' && isNumber(exp[i]))
		{
			exp.insert(i + 1, "*");
		}
		if (exp[i + 1] == 'e' && isNumber(exp[i]))
		{
			exp.insert(i + 1, "*");
		}
	}
	return exp;
}

double Clamp(double x, double low, double high)
{
	return x < low ? low : (x > high ? high : x);
}

double IfOverflowThenReset(double x, double low, double high)
{
	return (x < high) ? x : low;
}

double derivative(std::string exp, double x)
{
	double dx = 0.00001f;
	double f1 = evaluate(exp, x + dx);
	double f2 = evaluate(exp, x + 2 * dx);
	double f_1 = evaluate(exp, x - dx);
	double f_2 = evaluate(exp, x - 2 * dx);
	return 4 / 3 * (f1 - f_1) / (2 * dx) - 1 / 3 * (f2 - f_2) / (4 * dx);
}

bool FindZero(std::string exp, double* x)
{
	double g = *x;
	for(unsigned short i = 0; i < 100; i++)
	{
		double esp = 0.0001f;
		double f = evaluate(exp, g);
		double fp = derivative(exp, g);
		if ((fp <= 0.0001f) && (fp >= -0.0001f))
		{
			g += 0.0001f;
			continue;
		}
		g -= f / fp;
		if (abs(evaluate(exp, g)) < esp)
		{
			*x = g;
			return true;
		}			
	}
	return false;
}
