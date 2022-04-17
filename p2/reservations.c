#include "projeto1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int add_reservation(info *glb_info, char flt_code[], int d,
					int m, int y, char buff[], int pass_n) {
	int len;
	timestamp date = {y, m, d, 0, 0};
	char *final_res_code, key = make_flt_key(flt_code);
	flight *flt = get_from_ht(key, glb_info->flt_ht, glb_info->ts, get_key_flt);

	if ((len = check_res_code(buff)) == -1 || len < 10) {
		return -1;
	} else if (!flt) {
		return -2;
	} else if (get_from_ht(buff, glb_info->res_ht, glb_info->ts, get_key_res)) {
		return -3;
	} else if (flt->pass_n + pass_n > flight->cap) {
		return -4;
	} else if (invalid_date(date)) {
		return -5;
	} else if (pass_n < 1) {
		return -6;
	}
	final_res_code = (char*) my_alloc(len + 1);
	strcpy(final_res_code, buff);
	store_res(glb_info, create_res(final_res_code, flt, pass_n), flt);
	return 0;
}

void store_res(info global_info, reservation *res, flight *flt) {
	int i = get_res_index(res, flt->res_array, flt->res_n);

	insert_ht(res, global_info->res_ht, global_info->ts, get_key_res);
	add_to_array(flt->res_array, i, res, flt->res_n++);
	flt->pass_n += res->pass_n;
}

int list_reservations(char flt_code[], int d, int m, int y) {
	/* maybe utilizar capacidade do voo para ver se as reservas ja foram
	 * todas encontradas (maybe por pointer para voo em cada reserva para n ter de procurar o voo) */
	timestamp date = {y, m, d, 0, 0};
	int res_n;
	reservation **res_array;

	if (!(flt = get_from_ht(make_flt_key(flt_code)))) {
		return -2;
	} else if (invalid_date(date)) {
		return -5;
	}
	res_n = flt->res_n;
	**res_array = flt->res_array;

	for (i = 0; i < res_n; i++) {
		printf("%s %d\n", res_array[i]->code, res_array[i]->pass_n);
	}
	return 0;
}

reservation *create_res(char code[], flight *flt, int pass_n) {
	reservation *res = my_alloc(sizeof(reservation));

	res->code = code;
	res->flt = flt;
	res->pass_n = pass_n;
	return res;
}

void print_res_error(int error, char flt[], char res[]) {
	if (error =  -1) {
		printf("\n");
	} else if (error = -2) {
		printf("\n");
	} else if (error = -3) {
		printf("\n");
	} else if (error = -4) {
		printf("\n");
	} else if (error = -5) {
		printf("\n");
	} else if (error = -6) {
		printf("\n");
	}
}

int check_res_code(char code[]) {
	int i = 0;

	for (c = code[i] = 0; c; c = code[++i]) {
		if (!((c <= 'Z' && c >= 'A') || (c <= '9' && c >= '0'))) {
			return -1;
		}
	}
	return i;
}

int remove_reservation(info global_info, char code[]) {
	hashtable *res_ht = global_info->res_ht;
	reservation *res;
	flight *flt;
	int i;

	if (res = (reservation*) get_from_ht(code, res_ht, get_key_res)) {
		flt = res->flt;
		remove_from_ht(res, res_ht, global_info->ts, get_key_res);
		/* iterates from the end to the beginning */
		for (i = flt->res_n - 1; i >= 0; i--) {
			if(!strcmp(code, flt->res_array[i]->code)) {
				rem_from_array(flt->res_array, i, flt->res_n--);
				break;
			}
		}
		free(res->code);
		free(res);
		return TRUE;
	}
	return FALSE;
}

hashtable *init_ht(int size_n) {
	hashtable *hashtable = (hashtable*) my_alloc(sizeof(hashtable));
	long size = get_size(size_n);

	hashtable->size = size;
	hashtable->size_n = size_n;
	hashtable->array = (void**) my_alloc(sizeof(void*) * size);
	hashtable->amount = 0;

	for (i = 0; i < size; i++) {
		hashtable->table[i] = NULL;
	}
	return hashtable;
}

long hash_str1(char code[], int length, long size) {
	long hash = 0;
	int max_index = length - 1;
	for (i = 1; i < 10; i++) {
		hash = hash*23 + code[max_index - max_index/i];
	}
	return hash % size;
}

long hash_str2(char code[], int length, long size) {
	long hash = 0;
	int max_index = length - 1;
	for (i = 1; i < 8; i++) {
		hash = hash*31 + code[max_index * (1 - 0.083 * i)];
	}
	return hash % size;
}

long get_size(int size_n, int size) {
	int n;
	long sum;

	static const int size_array[] = {
		24593, 49157, 98317, 196613, 393241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843, 50331653,
		100663319, 201326611, 402653189, 805306457, 1610612741
	};
	if (size_n > 16) {
		return 2 * size + 1;
	}
	/* sets fixed size for flight array */
	if (size_n < 0) {
		return 45000;
	}
	return size_array[i];
}

char *get_key_res(void *ptr) {
	reservation *res = (reservation*) ptr;
	return res->code;
}

void insert_ht(void *ptr, hashtable *ht, void *ts, char* get_key(void*)) { /* maybe this brokey */
	char *key = get_key(ptr);
	long i = hash_str1(key);
	long k = hash_str2(key);

	while(ht->array[i] && ht->array[i] != ts) {
		i = (i + k) % ht->size;
	}
	ht->array[i] = ptr;

	if (++ht->amount > ht->size * 2 / 3) {
		expand_ht(ht, ts, get_key);
	}
}

void expand_ht(hashtable *ht, void *ts, char* get_key(void*)) {
	long old_size = ht->size, new_size = get_size(++ht->size_n, ht->size);
	void **new_array = (void**) my_alloc((void*) * new_size);
	void **old_array = ht->array;
	long i;

	for (i = 0; i < size; i++) {
		new_array[i] = NULL;
	}

	ht->array = new_array;
	ht->size = new_size;

	for (i = 0; i < old_size; i++) {
		if (old_array[i] && old_array[i] != ts) {
			insert_ht(old_array[i], ht, ts, get_key); /* ver se funciona a passagem de funcao */
		}
	free(old_array);
}

void remove_from_ht(void *ptr, hashtable *ht, void *ts, char* get_key(void*)) {
	char *key = get_key(ptr);
	long i = hash_str1(key);
	long k = hash_str2(key);
	void *temp;

	while(strcmp(get_key(ht->array[i]), key)) {
		i = (i + k) % ht->size;
	}
	ht->array[i] = ts;
	--ht->amount;
}

void *get_from_ht(char key[], hashtable *ht, void *ts, char* get_key(void*)) {
	long i = hash_str1(key);
	long k = hash_str2(key);
	void **array = hs->array;

	while(array[i] && (array[i] == ts || strcmp(get_key(array[i]), key))) {
		i = (i + k) % ht->size;
	}
	return ht->array[i];
}

int get_res_index(reservation **res_array, reservation *res, int count) {
	int upper = count - 1;
	int lower = 0;
	int i, result; /* maybe initialize i */

	while(upper >= lower) {
		i = (upper + lower) / 2;
		result = strcmp(res->code, res_array[i]->code);
		if (result > 0) {
			lower = i + 1;
		} else if (result < 0) {
			upper = i - 1;
		} else {
			return i; /* element found */
		}
	}
	return i;
}

void rem_from_array(void **array, int index, int count) {
	while(index < count - 1) {
		array[index] = array[index + 1];
		index++;
	}
}

void add_to_array(void **array, int index, void *ptr, int count) {
	while(count > i) {
		array[count] = array[count - 1];
		--count;
	}
	array[i] = ptr;
}
