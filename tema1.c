#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cel
{
	char *key;
	char *val;
	struct cel *nxt; /*pointer to the next element */
}cel_t;

typedef struct hashT
{
	int size;
	cel_t **list; /* list of buckets */
}Hash;

typedef struct position
{
	int x, y;
}Poz;

int init_hash(Hash **ht)
{
	Hash *h;
	h = malloc(sizeof(Hash));
	if(h == NULL)
		return 12;
	*ht = h;
	h->size = 64;
	h->list = calloc(h->size, sizeof(cel_t));
	if(h->list == NULL)
	{
		free(h);
		return 12;
	}
	return 0;
}

int hash_fct(int size, char *w)
{
	int i = 0, val = 0;
	while(w[i])
	{
		val = ((val * 73) + w[i]) & (size - 1); /* size e mereu o putere a lui 2 */
		i++;
	}
	return val;
}

int hash_insert(Hash **h, char *k, char *v)
{
	int bucket = hash_fct((*h)->size, k);
	cel_t *p = (*h)->list[bucket];
	/*aloc o celula*/
	cel_t *newcel = malloc(sizeof(cel_t));
	newcel->nxt = NULL;
	if(newcel == NULL)
		return 12;
	newcel->key = malloc(strlen(k) + 1);
	if(newcel->key == NULL)
		return 12;
	strcpy(newcel->key, k);

	newcel->val = malloc(strlen(v) + 1);
	if(newcel->val == NULL)
		return 12;
	strcpy(newcel->val, v);
	if(p == NULL)
	{
		(*h)->list[bucket] = newcel;
	}
	else
	{
		cel_t *ant = p;
		while(p != NULL)
		{
			ant = p;
			p = p->nxt;
		}
		ant->nxt = newcel;
	}
	return 0;
}

void destroy_hash(Hash **h)
{
	int i = 0;
	for(i = 0; i < (*h)->size; i++)
	{
		if((*h)->list[i] == NULL)
			continue;
		cel_t *p = (*h)->list[i];
		cel_t *ant = p;
		while(p != NULL)
		{
			ant = p;
			p = p->nxt;
			free(ant->key);
			free(ant->val);			
			free(ant);
		}
	}
	free((*h)->list);
	free((*h));
}

Poz search_cel(Hash **h, char *k)
{
	Poz rez;
	rez.x = rez.y = -1;
	int bucket = hash_fct((*h)->size, k);
	if((*h)->list[bucket] == NULL)
		return rez;
	cel_t *p = (*h)->list[bucket];
	while(p != NULL)
	{
		rez.y++;
		if(strcmp(p->key, k) == 0)
			{
				rez.x = bucket;
				return rez;
			}
		p = p->nxt;
	}
	return rez;

}

char *get_val(Hash **h, char *k)
{
	Poz r = search_cel(h, k);
	if(r.x == -1)
		return NULL;
	return ((*h)->list[r.x][r.y]).val;
}

void remove_cel(Hash **h, char *k)
{
	Poz r = search_cel(h, k);
	if(r.x == -1)
		return;
	cel_t *aux = &((*h)->list[r.x][r.y]);
	if(r.y == 0)
	{
		(*h)->list[r.x] = aux->nxt;
	}
	else
	{
		cel_t *ant = &((*h)->list[r.x][r.y - 1]);
		ant->nxt = aux->nxt;
	}
	free(aux->key);
	free(aux->val);
	free(aux);
}

int parse_param(int argc, char * argv[], Hash **h, char ***I, char **in, char **out)
{
	int i = 1, indI = 0, infile = 0, outfile = 0;
	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-' && argv[i][1] == 'D')
		{
			/*caz1 simbolu sa fie in urmatoru argv => poa sa fie "" sau ceva
			caz2 simbolu sa fie lipit de -D => poa sa fie "" sau ceva */
			if(strlen(argv[i]) == 2)
			{
				if(i < argc && strchr(argv[i + 1], '=') != NULL)
				{

					char *key = strtok(argv[i + 1], "=");
					char *val = strtok(NULL, "="); /* in val ar trebui verificat daca nu exista deja un alt define */
					hash_insert(h, key, val);
					i++;
				}
				else
				{
					hash_insert(h, argv[i + 1], "");
					i++;
				}
			}
			else /* lipit de -D */
			{
				char *aux;
				aux = argv[i] + 2;
				if(strchr(aux, '=') != NULL)
				{
					char *key = strtok(argv[i + 1], "=");
					char *val = strtok(NULL, "=");
					hash_insert(h, key, val);
				}
				else
				{
					hash_insert(h, aux, "");
				}
			}

		}
		else
		{
			if(argv[i][0] == '-' && argv[i][1] == 'I')
			{
				if(strlen(argv[i]) == 2)
				{
					(*I)[indI] = malloc(strlen(argv[i + 1]) + 1);
					strcpy((*I)[indI], argv[i + 1]);
					indI++;
					i++;
				}
				else
				{
					char *aux;
					aux = argv[i] + 2;
					(*I)[indI] = malloc(strlen(aux) );
					strcpy((*I)[indI], aux);
					indI++;
				}
			}
			else
			{
				if(argv[i][0] == '-' && argv[i][1] == 'o')
				{
					if(outfile == 1)
						return -1;/*eroare prea multe outfiles*/
					outfile = 1;
					if(strlen(argv[i]) == 2)
					{
						*out = malloc(strlen(argv[i + 1]) + 1);
						strcpy(*out, argv[i + 1]);
						i++;						
					}
					else
					{
						*out = malloc(strlen(argv[i]) - 1);
						strcpy(*out, argv[i] + 2);
					}

				}
				else
				{
					/*INFILE*/
					if(infile == 1 && outfile == 1)
						return -1;
					if(infile == 0)
					{
						infile = 1;
						*in = malloc(strlen(argv[i]) + 1);
						strcpy(*in, argv[i]);						
					}
					else
					{
						outfile = 1;
						*out = malloc(strlen(argv[i]) + 1);
						strcpy(*out, argv[i] );
					}

				}
			}

		}
	}
	return 0;
}

void destroy_Ipointers(char **I, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(I[i] != NULL)
			free(I[i]);
	}
	free(I);
}

void parse_define(Hash **h, char s[300], FILE *f);

void preproces_if(Hash **h, int iflag, FILE *f)
{
	char s[301], *tok, sdup[301];
	char delim[30] = " \t[]{}<>=+-*/%!&|^.,:;()\\";
	while(fgets(s, 300, f))
	{
		strcpy(sdup, s);

		tok = strtok(sdup, delim);
		if(strcmp(tok, "#endif\n") == 0) /* POSIBILE PROBLEME IN WINDOWS*/
			{break;}
		if(iflag == 1)
		{
			if(strcmp(tok, "#else\n") == 0 || strcmp(tok, "#elif\n") == 0)
				iflag = 0;
			else
			{
				if(strcmp(tok, "#define") == 0)
					parse_define(h, s, f);
				else
					printf("%s", s);
			}
		}
		else
		{
			if(strcmp(tok, "#else\n") == 0)
				iflag = 1;
			else
			{
				if(strcmp(tok, "#elif") == 0)
				{
					char *key = strtok(NULL, delim);
					key[strlen(key) - 1] = 0;
					char *val = get_val(h, key);
					if(val != NULL)
					{
						if(strcmp(val, "0") == 0)
							iflag = -1;
						else
							iflag = 1;
					}
					else
					{
						if(strcmp(key, "0") == 0)
							iflag = -1;
						else
							iflag = 1;
					}
				}
			}
		}
	}
}


void parse_define(Hash **h, char s[300], FILE *f)
{
	char sdup[301];
	strcpy(sdup, s);
	char delim[30] = " \t[]{}<>=+-*/%!&|^.,:;()\\";
	char *key = strtok(sdup, delim);
	key = strtok(NULL, delim);
	char *val = strtok(NULL, delim); 
	char valtot[100] = "";
	int lastind = 0, newind, indvaltot = 0;
	while(val != NULL)
	{
		if(val[strlen(val) - 1] == '\n')
			val[strlen(val) - 1] = 0;
		if(val[strlen(val) - 1] == '\r') /*PENTRU WINDOWS? */
			val[strlen(val) - 1] = 0;
		newind = val - sdup;
		if (lastind != 0){memcpy(valtot + indvaltot, s + lastind, newind - lastind);indvaltot += newind - lastind;}
		char *defr = get_val(h, val);
		if(defr == NULL)
		{
			memcpy(valtot + indvaltot, val, strlen(val));
			indvaltot += strlen(val);
		}
		else
		{
			memcpy(valtot + indvaltot, defr, strlen(defr));
			indvaltot += strlen(defr);
		}
		lastind = val - sdup + strlen(val);
		val = strtok(NULL, delim);
	}
	valtot[indvaltot] = 0; 
	hash_insert(h, key, valtot);

}
int preprocess(FILE *f, Hash **h, char **Ipointers);

void preprocess_ifdef(Hash **h, int ifd, char s[300], FILE *f, char **Ipointers)
{
	char delim[30] = " \t[]{}<>=+-*/%!&|^.,:;()\\";
	char sdup[300];
	strcpy(sdup, s);
	char *tok = strtok(sdup, delim);
	char *key = strtok(NULL, delim);
	key[strlen(key) - 1] = 0;
	char *val = get_val(h, key);
	int iflag = 0;
	if((ifd == 1 && val != NULL) || (ifd == 0 && val == NULL))
		iflag = 1;
	while(fgets(s, 300, f))
	{
		strcpy(sdup, s);
		tok = strtok(sdup, delim);
		if(strcmp(tok, "#endif\n") == 0  || strcmp(tok, "#endif") == 0) /* POSIBILE PROBLEME IN WINDOWS*/
			break;
		if(strcmp(tok, "#else\n") == 0)
		{
			if(iflag == 0)
				iflag = 1;
			else iflag = 0;
			continue;
		}
		if(iflag == 1)
			{
				if(strcmp(tok, "#else\n") == 0) /* endline diferit in windows? */
				{
					iflag = 0;
					continue;
				}
				else
				if(strcmp(tok, "#define") == 0)
				{
					parse_define(h, s, f);
				}
				else
				{
					if(strcmp(tok, "#undef") == 0)
					{
						key = strtok(NULL, delim);
						key[strlen(key) - 1] = 0; /* DE ADAUGAT CEVA PT WINDOWS? */
						remove_cel(h, key);
					}
					else
						if(strcmp(tok, "#ifdef") == 0)
						{
							preprocess_ifdef(h, 1, s, f, Ipointers);
						}
						else
							if(strcmp(tok, "#ifndef") == 0)
							{
								preprocess_ifdef(h, 0, s, f, Ipointers);
							}
							else
								if(strcmp(tok, "#include") == 0)
								{
									tok = strtok(NULL, " \"");
									char cale[40] = "_test/inputs/";
									char aux[40] = "";
									memcpy(aux, cale, strlen(cale) + 1);
									strcat(aux, tok);
									FILE *nf = fopen(aux, "r");
									if(nf == NULL)
									{
										if(Ipointers[0] != NULL)
										{
											memcpy(aux, Ipointers[0], strlen(Ipointers[0]) + 1);
											strcat(aux, "/");
											strcat(aux, tok);
											nf = fopen(aux, "r"); 
											if(nf == NULL)
												return;
										}
										else
											return;
							}
								preprocess(nf, h, Ipointers);
								}
								else
									if(strcmp(tok, "#if") == 0)
									{

										int inif = 0;
										char *key = strtok(NULL, delim);
										key[strlen(key) - 1] = 0;

										char *val = get_val(h, key);
										if(val != NULL)
										{
											if(strcmp(val, "0") == 0)
												inif = -1;
											else
												inif = 1;
										}
										else
										{
											if(strcmp(key, "0") == 0)
												inif = -1;
											else
												inif = 1;
										}
										preproces_if(h, inif, f);
									}
									else									
										printf("%s", s);
				}
			}
	}
}

int preprocess(FILE *f, Hash **h, char **Ipointers)
{
	char s[1000], buf[1000], sdup[300];
	char delim[30] = " \t[]{}<>=+-*/%!&|^,:;()\\";
	int inif = -1;
	int indbuf = 0, st, fi, needs_replacing;
	while(fgets(s, 300, f))
	{
		strcpy(sdup, s);
		char *tok = strtok(sdup, delim);
		if(strcmp(tok, "#define") == 0)
			parse_define(h, s, f);
		else 
		if(strcmp(tok, "#undef") == 0)
		{
			char *key = strtok(NULL, delim);
			if(key[strlen(key) - 1] == '\n')
				key[strlen(key) - 1] = 0;
			if(key[strlen(key) - 1] == '\r') /*PENTRU WINDOWS?*/
				key[strlen(key) - 1] = 0;
			remove_cel(h, key);
		}
		else 
		{
			if(strcmp(tok, "#if") == 0)
			{
				char *key = strtok(NULL, delim);
				key[strlen(key) - 1] = 0;
				char *val = get_val(h, key);
				if(val != NULL)
				{
					if(strcmp(val, "0") == 0)
						inif = -1;
					else
						inif = 1;
				}
				else
				{
					if(strcmp(key, "0") == 0)
						inif = -1;
					else
						inif = 1;
				}
				preproces_if(h, inif, f);
			}
			else
			{
				if(strcmp(tok, "#ifdef") == 0)
					preprocess_ifdef(h, 1, s, f, Ipointers);
				else
					if(strcmp(tok, "#ifndef") == 0)
						preprocess_ifdef(h, 0, s, f, Ipointers);
					else
						if(strcmp(tok, "#include") == 0) /* AICI TREBUIE PUS UN FOR CA SA iTEREZ PRIN TOATE IPOINTERS */
						{
							tok = strtok(NULL, " \"");
							char cale[40] = "_test/inputs/";
							char aux[40] = "";
							memcpy(aux, cale, strlen(cale) + 1);
							strcat(aux, tok);
							FILE *nf = fopen(aux, "r");
							if(nf == NULL)
							{
								if(Ipointers[0] != NULL)
								{
									memcpy(aux, Ipointers[0], strlen(Ipointers[0]) + 1);
									strcat(aux, "/");
									strcat(aux, tok);
									nf = fopen(aux, "r"); 
									if(nf == NULL)
										return 12;
								}
								else
									return 12;
							}
							preprocess(nf, h, Ipointers);
						}
						else
						{
							st = 0;
							fi = 0;
							needs_replacing = 0;
							indbuf = 0;
							while(tok)
							{
								char *val = get_val(h, tok);
								if(val == NULL)
									fi+= strlen(tok) + 1;
								else
								{	
									needs_replacing = 1;
									fi = tok - sdup - st;
									memcpy(buf + indbuf, s + st, fi); /* copiaza tot stringu pana la cheia gasita*/
									memcpy(buf + indbuf + fi, val, strlen(val));
									indbuf += fi + strlen(val);
									st += fi + strlen(tok);
								}

								tok = strtok(NULL, delim);
							}
							if(needs_replacing == 0)
								printf("%s", s);
							else 
							{
								fi = strlen(s) - st + 1;
								memcpy(buf + indbuf, s + st, fi);
								printf("%s", buf);
							}
						}
			}
		}	
	}
	return 0;
}

int main(int argc, char* argv[])
{
	char **Ipointers = calloc(argc, sizeof(char *)), *in = NULL, *out = NULL;
	int ret;
	if(Ipointers == NULL)
		return 12;

	Hash *h;
	init_hash(&h);
	ret = parse_param(argc, argv, &h, &Ipointers, &in, &out); 
	if(ret != 0)
		goto end;
	if(in != NULL && freopen(in, "r", stdin) == NULL)
		{
			ret = 11;
			goto end;
		}
	if(out != NULL)
		freopen(out, "w", stdout);

	ret = preprocess(stdin, &h, Ipointers);
	
	end:
	destroy_hash(&h);
	destroy_Ipointers(Ipointers, argc);
	if(in != NULL)
		free(in);
	if(out != NULL)
		free(out);
	fclose(stdin);
	return ret;
}



