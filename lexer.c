#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "utils.h"

Token *tokens; // single linked list of tokens
Token *lastTk; // the last token in list

int line = 1; // the current line in the input file

// adds a token to the end of the tokens list and returns it
// sets its code and line
Token *addTk(int code)
{
	Token *tk = safeAlloc(sizeof(Token));
	tk->code = code;
	tk->line = line;
	tk->next = NULL;
	if (lastTk)
	{
		lastTk->next = tk;
	}
	else
	{
		tokens = tk;
	}
	lastTk = tk;
	return tk;
}

char *extract(const char *begin, const char *end) {
  // Make sure begin and end are not NULL
  if (begin == NULL || end == NULL) {
    printf("NULL pointer passed to extract function.\n");
    return NULL;
  }
  // Make sure end is not smaller than begin
  if (end < begin) {
    printf("End pointer is smaller than begin pointer.\n");
    return NULL;
  }
  int length = end - begin;
  char *tempStr = safeAlloc((length + 1) * sizeof(char));
  memcpy(tempStr, begin, length);
  tempStr[length] = '\0';
  printf("Extracted : %s\n", tempStr);
  return tempStr;
}

Token *tokenize(const char *pch)
{
	const char *start;
	Token *tk;
	int quoteCount = 0;
	for(;;)
	{
		switch (*pch)
		{
		case ' ':

		case '\t':
			pch++;
			break;

		case '\r': // handles different kinds of newlines (Windows: \r\n, Linux: \n, MacOS, OS X: \r or \n)
			if (pch[1] == '\n')
				pch++;
			// fallthrough to \n

		case '\n':
			line++;
			pch++;
			break;

		case '\0':
			addTk(END);
			return tokens;

		case ',':
			addTk(COMMA);
			pch++;
			break;

		case '(':
			addTk(LEFTPAR);
			pch++;
			break;

		case ')':
			addTk(RIGHTPAR);
			pch++;
			break;

		case '{':
			addTk(LACC);
			pch++;
			break;

		case '}':
			addTk(RACC);
			pch++;
			break;

        case '[':
            addTk(LBRACKET);
            pch++;
            break;

        case ']':
            addTk(RBRACKET);
            pch++;
            break;

		case ';':
			addTk(SEMICOLON);
			pch++;
			break;

		case '+':
			addTk(ADD);
			pch++;
			break;

		case '-':
            addTk(SUB);
            pch++;
            break;

        case '!':
            if (pch[1] == '=')
            {
                addTk(NOTEQ);
                pch += 2;
            }
            else
            {
                addTk(NOT);
                pch += 1;
            }
            break;

        case '|':
            if (pch[1] == '|')
            {
                addTk(OR);
                pch += 2;
            } else err("It is required to use ||!");
        break;

        case '<':
        if (pch[1] == '=')
        {
          addTk(LESSEQ);
          pch += 2;
        }
        else
        {
          addTk(LESS);
          pch++;
        }
        break;

		case '*':
			addTk(MUL);
			pch++;
			break;

        case '>':
        if (pch[1] == '=')
        {
          addTk(GREATEREQ);
          pch += 2;
        }
        else
        {
          addTk(GREATER);
          pch++;
        }
        break;

		case '=':
			if (pch[1] == '=')
			{
				addTk(EQUAL);
				pch += 2;
			}
			else
			{
				addTk(ASSIGN);
				pch++;
			}
			break;

		case '&':
			if (pch[1] == '&')
			{
				addTk(AND);
				pch += 2;
			}
			else {
                err("no valid &&!");
			}
			break;

		case '/':
			if (pch[1] == '/')
			{
				addTk(LINECOMMENT);
				while (*pch != '\0' && *pch != '\n')
				{
					pch++;
				}
			}
			else
			{
				addTk(DIV);
				pch++;
			}
			break;

        case '.':
      	addTk(DOT);
      	pch++;
      	break;

		default:
			if (isalpha(*pch) || *pch == '_')
			{
				for(start = pch++; isalnum(*pch) || *pch == '_'; pch++)
				{
				}

				char *text = extract(start, pch);

				if (strcmp(text, "char") == 0)
                    addTk(TYPE_CHAR);
                else if (strcmp(text, "int") == 0)
                    addTk(TYPE_INT);
                else if (strcmp(text, "double") == 0)
                    addTk(TYPE_DOUBLE);
                else if (strcmp(text, "while") == 0)
                    addTk(WHILE);
                else if (strcmp(text, "if") == 0)
                    addTk(IF);
                else if (strcmp(text, "else") == 0)
                    addTk(ELSE);
                else if (strcmp(text, "struct") == 0)
                    addTk(STRUCT);
                else if (strcmp(text, "return") == 0)
                    addTk(RETURN);
                else if (strcmp(text, "void") == 0)
                    addTk(VOID);
                else {
                    tk = addTk(ID);
                    tk->text = text;
                    }
			}
			else if (isalpha(*pch) || *pch == '"' || *pch == '\'')
			{
				short isSimpleQuote = 0;
				pch++;

				for(start = pch++; isalnum(*pch) || *pch == '"' || *pch == '\''; pch++)
				{
					if (*pch == '\'')
					{
						isSimpleQuote = 1;
					}
					else if(*pch == '"')
                    {
                        quoteCount++;
                    }

				}

				if (isSimpleQuote == 1)
                {
                    char *character = extract(start, pch);
                    if (pch - start > 2)
                    {
                        err("Too long for a char: '%s'", character);
                        return NULL;
                    }
                    else if(*(character) == '\'')
                    {
                        err("Invalid character '");
                        return NULL;
                    }
                    tk = addTk(CHAR);
                    tk->c = *character;
                }
				else if(quoteCount != 1)
                {
                    err("Invalid String");
                    return NULL;
                }
                else
				{
					char *text = extract(start, pch - 1);
					tk = addTk(STRING);
					tk->text = text;
				}
			}
            else if (isdigit(*pch))
            {
                int hasDot = 0;
                int hasExponential = 0;

                for(start = pch; isdigit(*pch) || *pch == '.'; pch++)
                    {
                        if (*pch == '.') {
                        if (hasDot) {
                        // Multiple dots are not allowed
                        err("Invalid double literal: multiple dots");
                        return NULL;
                    }
                hasDot = 1;
                }
            }

            // Must have at least one digit after the dot
                if (hasDot && !isdigit(pch[-1]))
                {
                    err("Invalid double literal: missing digits after dot");
                    return NULL;
                }

            // Check f optional exponential part
                if (*pch == 'e' || *pch == 'E') {
                hasExponential = 1;
                pch++; // Move past 'e'  'E'

            // Check f optional sign
                if (*pch == '+' || *pch == '-')
                pch++; // Move past sign

            // Check f digits after exponent
                if (!isdigit(*pch)) {
                err("Invalid double literal: missing digits after exponent");
                return NULL;
                }

            // Consume digits after exponent
                while (isdigit(*pch))
                pch++;
                }

            char *text = extract(start, pch);
            if(hasDot || hasExponential)
            {
                tk = addTk(DOUBLE);
                tk->d = strtod(text, NULL);
            }

				else
				{
					tk = addTk(INT);
					tk->i = atoi(text); //converts the text to int
				}
			}
			else
			{
				err("invalid char: %c (%d)", *pch, *pch);
			}
		}
	}
}

void showTokens(Token *tokens) {
  for(const Token *tk = tokens; tk; tk = tk->next) {
    printf("%d\t", tk->line);
    switch (tk->code) {
      case TYPE_INT:
        printf("TYPE_INT");
        break;
      case TYPE_CHAR:
        printf("TYPE_CHAR");
        break;
      case TYPE_DOUBLE:
        printf("TYPE_DOUBLE");
        break;
      case ID:
        printf("ID : %s", tk->text);
        break;
      case LEFTPAR:
        printf("LPAR");
        break;
      case RIGHTPAR:
        printf("RPAR");
        break;
      case LACC:
        printf("LACC");
        break;
      case RACC:
        printf("RACC");
        break;
      case LBRACKET:
        printf("LBRACKET");
        break;
      case RBRACKET:
        printf("RBRACKET");
        break;
      case COMMA:
      	printf("COMMA");
      	break;
      case SEMICOLON:
        printf("SEMICOLON");
        break;
      case DOT:
       printf("DOT");
       break;
      case INT:
        printf("INT : %d", tk->i);
        break;
      case DOUBLE:
        printf("DOUBLE : %f", tk->d);
        break;
      case STRING:
        printf("STRING : %s", tk->text);
        break;
      case CHAR:
        printf("CHAR : %c", tk->c);
        break;
      case WHILE:
        printf("WHILE");
        break;
      case DIV:
        printf("DIV");
        break;
      case ADD:
        printf("ADD");
        break;
      case AND:
        printf("AND");
        break;
      case MUL:
        printf("MUL");
        break;
      case IF:
        printf("IF");
        break;
      case ASSIGN:
        printf("ASSIGN");
        break;
      case EQUAL:
        printf("EQUAL");
        break;
      case RETURN:
        printf("RETURN");
        break;
      case END:
        printf("END");
        break;
      case ELSE:
        printf("ELSE");
        break;
      case STRUCT:
        printf("STRUCT");
        break;
      case VOID:
        printf("VOID");
        break;
      case SUB:
        printf("SUB");
        break;
      case OR:
        printf("");
        break;
      case NOT:
        printf("NOT");
        break;
      case NOTEQ:
        printf("NOTEQ");
        break;
      case LESS:
        printf("LESS");
        break;
      case LESSEQ:
        printf("LESSEQ");
        break;
      case GREATER:
        printf("GREATER");
        break;
      case GREATEREQ:
        printf("GREATEREQ");
        break;
      case SPACE:
      	printf("SPACE");
      	break;
      case LINECOMMENT:
      	printf("LINECOMMENT");
      	break;

      default:
      	printf("No token code match f token at line : %d\n", tk->line);
        break;
    }
    printf("\n");
  }
}
