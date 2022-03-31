#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "projeto1.h"

int global_flight_amount = 0;
int global_srtd_flt_amount = 0;
flight global_flt_list[MAX_FLT];
flight global_srtd_flt_list[MAX_FLT];

void add_flight(flight flight, timestamp dep_date) {
	flight.dep_date = dep_date;
	global_flt_list[global_flight_amount++] = flight;
}

int invalid_flt_args(flight new_f, timestamp dep_dt) {
	if (invalid_flt_code(new_f.code)) {
		printf("invalid flight code\n");
	} else if (is_flight(new_f.code, dep_dt)) {
		printf("flight already exists\n");
	} else if (!is_airport(new_f.origin)) {
		printf("%s: no such airport ID\n", new_f.origin);
	} else if (!is_airport(new_f.destin)) {
		printf("%s: no such airport ID\n", new_f.destin);
	} else if (global_flight_amount >= MAX_FLT) {
		printf("too many flights\n");
	} else if (invalid_date(dep_dt)) {
		printf("invalid date\n");
	} else if (invalid_duration(new_f.dura.h, new_f.dura.min)) {
		printf("invalid duration\n");
	} else if (new_f.cap > MAX_CAP || new_f.cap < MIN_CAP) {
		printf("invalid capacity\n");
	} else {
		return FALSE;
	} 
	return TRUE;
}
  
int is_flight(char str[], timestamp dt) {
	int i;
	flight flt;

	for (i = 0; i < global_flight_amount; i++) {
		flt = global_flt_list[i];
		if (!strcmp(flt.code, str) && same_day(flt.dep_date, dt)) {
			return TRUE;
		}
	}
	return FALSE;
}

void list_flights() {
	int i;
	flight flt;

	for (i = 0; i < global_flight_amount; i++) {
		flt = global_flt_list[i];
		printf("%s %s %s %02d-%02d-%d %02d:%02d\n",
			flt.code, flt.origin, flt.destin, flt.dep_date.d,
			flt.dep_date.mth, flt.dep_date.y, flt.dep_date.h,
		 	flt.dep_date.min);
	}
}

void list_departing_flights() {
	int i;
	flight flt;

	for (i = 0; i < global_srtd_flt_amount; i++) {
		flt = global_srtd_flt_list[i];
		printf("%s %s %02d-%02d-%d %02d:%02d\n",
			flt.code, flt.destin, flt.dep_date.d, flt.dep_date.mth,
		 	flt.dep_date.y, flt.dep_date.h, flt.dep_date.min);
	}
}

void list_arriving_flights() {
	int i;
	flight flt;

	for (i = 0; i < global_srtd_flt_amount; i++) {
		flt = global_srtd_flt_list[i];
		printf("%s %s %02d-%02d-%d %02d:%02d\n",
			flt.code, flt.origin, flt.dep_date.d, flt.dep_date.mth,
		 	flt.dep_date.y, flt.dep_date.h, flt.dep_date.min);
	}
}

int invalid_flt_code(char cd[]) {
	int i = 3;

	if (!isupper(cd[0]) || !isupper(cd[1]) || cd[2] > '9' || cd[2] < '1') {
		return TRUE;
	}
	for (i = 3; cd[i]; ++i) {
		if (!isdigit(cd[i])) {
			return TRUE;
		}
	}
	return FALSE;
}

int invalid_duration(int hours, int mins) {
	int max_d = MAX_FLT_DURATION;

	return (hours > max_d || (hours == max_d && mins));
}

int get_flts_departing(char id[]) {
	int i;
	flight flight;

	global_srtd_flt_amount = 0; /* resets counter */

	for (i = 0; i < global_flight_amount; i++) {
		flight = global_flt_list[i];
		if (!strcmp(flight.origin, id)) {
			global_srtd_flt_list[global_srtd_flt_amount++] = flight;
		}
	}
	return global_srtd_flt_amount;
}

void get_flts_arriving(char id[]) {
	int i;
	flight flight;

	global_srtd_flt_amount = 0; /* resets counter */

	for (i = 0; i < global_flight_amount; i++) {
		flight = global_flt_list[i];
		if (!strcmp(flight.destin, id)) {
			/* changes flight date to its arrival date */
			flight.dep_date = get_date_arrival(flight);
			global_srtd_flt_list[global_srtd_flt_amount++] = flight;
		}
	}
}

timestamp get_date_arrival(flight flt) {
	int unix1, unix2;

	unix1 = get_unix_time(flt.dep_date);
	unix2 = flt.dura.h * 60 + flt.dura.min;
	return unix_to_regular(unix1 + unix2);
}

void sort_flights() {
	timestamp d;
	flight temp;
	int i, j;

	for (i = 1; i < global_srtd_flt_amount; i++) {
		d = global_srtd_flt_list[i].dep_date;
		j = i;
		temp = global_srtd_flt_list[i];
		while (j > 0 && datecmp(d, global_srtd_flt_list[j - 1].dep_date) < 0) {
			global_srtd_flt_list[j] = global_srtd_flt_list[j - 1];
			j--;
		}
		global_srtd_flt_list[j] = temp; 
	}
}
