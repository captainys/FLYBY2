#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "impulse2g.h"


static char *F16[]={
	"Surf",
	"VER 0.00 -0.10 7.00",
	"VER 0.00 -0.40 4.90 R",
	"VER 0.00 0.50 4.90 R",
	"VER 0.00 -0.40 4.20",
	"VER 0.00 0.60 4.20 R",
	"VER 0.00 1.10 3.60 R",
	"VER 0.00 1.30 2.80 R",
	"VER 0.00 1.10 1.80 R",
	"VER 0.00 0.90 1.00 R",
	"VER -0.50 0.00 4.90 R",
	"VER -0.60 0.00 4.20 R",
	"VER 0.00 -0.20 2.50",
	"VER 0.00 -1.00 2.50",
	"VER -0.80 0.00 2.50",
	"VER -0.80 0.30 0.60 R",
	"VER -0.80 -0.30 0.60",
	"VER -0.53 0.50 2.56 R",
	"VER -0.38 0.43 4.21 R",
	"VER -0.32 0.36 4.91 R",
	"VER -0.59 0.65 0.77 R",
	"VER -0.40 -0.35 4.20 R",
	"VER -0.31 -0.36 4.90 R",
	"VER -0.72 -0.12 2.50",
	"VER -0.50 -0.90 2.50",
	"VER -0.70 -0.70 2.50",
	"VER -0.80 -0.30 2.50",
	"VER -0.33 0.50 3.41 R",
	"VER -0.38 0.64 1.68 R",
	"VER -0.45 1.02 2.68 R",
	"VER -0.27 0.93 3.51 R",
	"VER -0.34 0.90 1.73 R",
	"VER 0.00 0.70 -2.80",
	"VER -1.40 0.00 0.70",
	"VER -1.40 0.00 0.00",
	"VER -4.90 0.00 -2.80",
	"VER -4.90 0.00 -3.90",
	"VER -1.40 0.00 -3.90",
	"VER 0.00 0.35 -7.70 R",
	"VER -0.25 0.24 -7.70",
	"VER -0.35 0.00 -7.70",
	"VER -0.25 -0.25 -7.70 R",
	"VER 0.00 -0.35 -7.70 R",
	"VER 0.00 0.70 -7.00",
	"VER -0.50 0.49 -7.00",
	"VER -0.70 0.00 -7.00",
	"VER -0.50 -0.50 -7.00 R",
	"VER 0.00 -0.70 -7.00 R",
	"VER -1.00 0.00 -7.40",
	"VER -0.70 0.00 -7.40",
	"VER -1.00 0.30 -4.90",
	"VER -1.00 -0.10 -4.90",
	"VER -0.70 0.30 -4.90",
	"VER -0.70 -0.10 -4.90",
	"VER 0.00 0.59 -4.90",
	"VER 0.00 0.80 0.00 R",
	"VER -1.00 0.30 -2.80",
	"VER -1.00 -0.10 -2.80",
	"VER -1.10 0.30 0.00 R",
	"VER -0.56 0.66 -2.80",
	"VER -0.53 0.75 0.00 R",
	"VER -0.37 0.57 -4.90 R",
	"VER -1.00 0.00 -3.90",
	"VER -1.00 0.30 -3.90",
	"VER -0.70 -0.70 0.60 R",
	"VER -0.50 -0.90 0.60 R",
	"VER 0.00 -1.00 0.60 R",
	"VER -0.70 -0.70 -4.90 R",
	"VER -0.50 -0.90 -4.90 R",
	"VER 0.00 -1.00 -4.90 R",
	"VER 0.00 1.40 -4.50",
	"VER 0.00 3.50 -6.80",
	"VER 0.00 3.50 -8.10",
	"VER 0.00 1.40 -7.30",
	"VER 0.00 0.70 -7.30",
	"VER -2.80 -0.40 -7.10",
	"VER -2.80 -0.40 -6.30",
	"VER -1.00 0.00 -5.30",
	"VER -2.57 -0.32 -7.42",
	"VER -4.90 0.10 -4.00",
	"VER -5.10 0.10 -4.00",
	"VER -5.10 0.10 -1.60",
	"VER -4.90 0.10 -2.80",
	"VER -4.90 -0.10 -4.00",
	"VER -5.10 -0.10 -4.00",
	"VER -5.10 -0.10 -1.60",
	"VER -4.90 -0.10 -2.80",
	"VER -0.58 -0.83 -3.60 R",
	"VER -0.58 -0.83 -4.90 R",
	"VER -0.72 -1.40 -3.90 R",
	"VER -0.69 -1.32 -4.90 R",
	"VER -2.80 -0.40 -6.61",
	"VER -1.00 0.00 -5.80",
	"VER 0.50 0.00 4.90 R",
	"VER 0.60 0.00 4.20 R",
	"VER 0.80 0.00 2.50",
	"VER 0.80 0.30 0.60 R",
	"VER 0.80 -0.30 0.60",
	"VER 0.53 0.50 2.56 R",
	"VER 0.38 0.43 4.21 R",
	"VER 0.32 0.36 4.91 R",
	"VER 0.59 0.65 0.77 R",
	"VER 0.40 -0.35 4.20 R",
	"VER 0.31 -0.36 4.90 R",
	"VER 0.72 -0.12 2.50",
	"VER 0.50 -0.90 2.50",
	"VER 0.70 -0.70 2.50",
	"VER 0.80 -0.30 2.50",
	"VER 0.33 0.50 3.41 R",
	"VER 0.38 0.64 1.68 R",
	"VER 0.45 1.02 2.68 R",
	"VER 0.27 0.93 3.51 R",
	"VER 0.34 0.90 1.73 R",
	"VER 1.40 0.00 0.70",
	"VER 1.40 0.00 0.00",
	"VER 4.90 0.00 -2.80",
	"VER 4.90 0.00 -3.90",
	"VER 1.40 0.00 -3.90",
	"VER 0.25 0.24 -7.70",
	"VER 0.35 0.00 -7.70",
	"VER 0.25 -0.25 -7.70 R",
	"VER 0.50 0.49 -7.00",
	"VER 0.70 0.00 -7.00",
	"VER 0.50 -0.50 -7.00 R",
	"VER 1.00 0.00 -7.40",
	"VER 0.70 0.00 -7.40",
	"VER 1.00 0.30 -4.90",
	"VER 1.00 -0.10 -4.90",
	"VER 0.70 0.30 -4.90",
	"VER 0.70 -0.10 -4.90",
	"VER 1.00 0.30 -2.80",
	"VER 1.00 -0.10 -2.80",
	"VER 1.10 0.30 0.00 R",
	"VER 0.56 0.66 -2.80 R",
	"VER 0.53 0.75 0.00 R",
	"VER 0.37 0.57 -4.90 R",
	"VER 1.00 0.00 -3.90",
	"VER 1.00 0.30 -3.90",
	"VER 0.70 -0.70 0.60 R",
	"VER 0.50 -0.90 0.60 R",
	"VER 0.70 -0.70 -4.90 R",
	"VER 0.50 -0.90 -4.90 R",
	"VER 2.80 -0.40 -7.10",
	"VER 2.80 -0.40 -6.30",
	"VER 1.00 0.00 -5.30",
	"VER 2.57 -0.32 -7.42",
	"VER 4.90 0.10 -4.00",
	"VER 5.10 0.10 -4.00",
	"VER 5.10 0.10 -1.60",
	"VER 4.90 0.10 -2.80",
	"VER 4.90 -0.10 -4.00",
	"VER 5.10 -0.10 -4.00",
	"VER 5.10 -0.10 -1.60",
	"VER 4.90 -0.10 -2.80",
	"VER 0.58 -0.83 -3.60 R",
	"VER 0.58 -0.83 -4.90 R",
	"VER 0.72 -1.40 -3.90 R",
	"VER 0.69 -1.32 -4.90 R",
	"VER 2.80 -0.40 -6.61",
	"VER 1.00 0.00 -5.80",
	"FAC",
	"COL 14700",
	"NOR -0.17 0.47 4.56 -0.40 0.91 0.07",
	"VER 2 4 17 18",
	"END",
	"FAC",
	"COL 14700",
	"NOR -0.45 0.20 4.56 -0.89 0.44 0.07",
	"VER 9 10 17 18",
	"END",
	"FAC",
	"COL 14701",
	"NOR -0.58 0.23 3.37 -0.89 0.45 0.06",
	"VER 10 13 16 17",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.68 0.36 1.61 -0.88 0.47 0.03",
	"VER 13 14 19 16",
	"END",
	"FAC",
	"COL 14700",
	"NOR -0.45 -0.18 4.55 -0.88 -0.47 0.00",
	"VER 9 10 20 21",
	"END",
	"FAC",
	"COL 14700",
	"NOR -0.18 -0.38 4.55 -0.13 -0.99 0.00",
	"VER 1 3 20 21",
	"END",
	"FAC",
	"COL 14701",
	"NOR -0.63 -0.12 3.35 -0.87 -0.50 0.00",
	"VER 10 13 22 20",
	"END",
	"FAC",
	"COL 14701",
	"NOR -0.28 -0.27 3.35 -0.12 -0.99 0.00",
	"VER 3 11 22 20",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.77 -0.14 1.87 -0.83 -0.55 0.08",
	"VER 13 22 15",
	"END",
	"FAC",
	"COL 2114",
	"NOR -0.45 -0.54 2.50 0.00 0.00 1.00",
	"VER 11 22 25 24 23 12",
	"END",
	"FAC",
	"COL 9513",
	"NOR -0.11 0.25 5.60 -0.38 0.89 0.25",
	"VER 2 18 0",
	"END",
	"FAC",
	"COL 9513",
	"NOR -0.27 0.09 5.60 -0.87 0.43 0.23",
	"VER 9 18 0",
	"END",
	"FAC",
	"COL 9513",
	"NOR -0.27 -0.15 5.60 -0.87 -0.46 0.19",
	"VER 0 9 21",
	"END",
	"FAC",
	"COL 9513",
	"NOR -0.10 -0.29 5.60 -0.13 -0.98 0.14",
	"VER 0 21 1",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.39 0.74 3.04 -0.97 0.10 0.22",
	"VER 26 16 28 29",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.18 1.09 3.15 -0.55 0.84 0.06",
	"VER 5 6 28 29",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.43 0.76 2.16 -0.98 0.17 -0.09",
	"VER 16 27 30 28",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.20 1.08 2.25 -0.55 0.83 0.10",
	"VER 6 7 30 28",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.09 0.88 3.77 -0.57 0.63 0.52",
	"VER 5 29 4",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.20 0.68 3.71 -0.92 0.04 0.38",
	"VER 4 29 26",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.11 0.97 1.51 -0.46 0.86 -0.21",
	"VER 7 8 30",
	"END",
	"FAC",
	"COL 31",
	"NOR -0.24 0.81 1.47 -0.89 0.22 -0.41",
	"VER 30 8 27",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.38 0.67 1.50 -0.42 0.90 0.11",
	"VER 16 27 8 19",
	"END",
	"FAC",
	"COL 14701",
	"NOR -0.31 0.51 3.59 -0.41 0.91 0.07",
	"VER 16 17 4 26",
	"END",
	"FAC",
	"COL 11403",
	"NOR -3.15 0.00 -2.65 0.00 0.00 0.00",
	"VER 33 34 35 36",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.00 0.10 1.27 -0.42 0.90 0.14",
	"VER 14 32 13",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.00 -0.10 1.27 -0.42 -0.90 0.14",
	"VER 13 15 32",
	"END",
	"FAC",
	"COL 5285",
	"NOR -0.19 0.44 -7.35 -0.36 0.83 -0.43",
	"VER 42 37 38 43",
	"END",
	"FAC",
	"COL 5285",
	"NOR -0.45 0.18 -7.35 -0.84 0.34 -0.42",
	"VER 38 39 44 43",
	"END",
	"FAC",
	"COL 5285",
	"NOR -0.45 -0.19 -7.35 -0.84 -0.34 -0.42",
	"VER 39 40 45 44",
	"END",
	"FAC",
	"COL 5285",
	"NOR -0.19 -0.45 -7.35 -0.34 -0.84 -0.42",
	"VER 40 41 46 45",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.00 0.07 -5.73 -1.00 0.00 0.00",
	"VER 49 50 47",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.85 0.15 -6.15 0.00 0.99 -0.12",
	"VER 51 49 47 48",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.85 -0.05 -6.15 0.00 -1.00 -0.04",
	"VER 50 52 48 47",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.27 0.73 -1.40 -0.09 1.00 0.00",
	"VER 54 31 58 59",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.80 0.50 -1.40 -0.62 0.78 0.00",
	"VER 57 55 58 59",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.23 0.63 -3.85 -0.07 1.00 0.00",
	"VER 31 53 60 58",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.66 0.46 -3.85 -0.63 0.77 0.00",
	"VER 55 51 60 58",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.63 0.26 -6.30 -0.92 0.38 -0.05",
	"VER 43 44 51",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.52 0.45 -5.60 -0.63 0.77 0.01",
	"VER 60 51 43",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.29 0.59 -6.30 -0.39 0.92 -0.01",
	"VER 42 43 60",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.12 0.62 -5.60 -0.05 1.00 0.05",
	"VER 42 60 53",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.23 0.15 -1.67 -0.71 0.71 0.00",
	"VER 55 57 33 36",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.13 0.10 -3.53 0.00 0.97 -0.26",
	"VER 55 61 36",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.00 0.12 -3.87 -1.00 0.00 0.00",
	"VER 55 62 49 50 56 61",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.90 0.30 -4.20 0.00 1.00 0.00",
	"VER 55 49 51",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.20 0.78 0.59 -0.35 0.93 -0.09",
	"VER 8 54 19",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.37 0.73 0.26 -0.09 0.99 0.12",
	"VER 19 54 59",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.64 0.57 0.46 -0.86 0.52 0.00",
	"VER 19 59 14",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.81 0.45 0.20 -0.59 0.75 0.29",
	"VER 14 59 57",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.10 0.20 0.43 -0.41 0.89 0.20",
	"VER 57 32 14",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.30 0.10 0.23 -0.71 0.71 0.00",
	"VER 32 57 33",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.13 -0.03 -3.53 0.00 -1.00 -0.09",
	"VER 61 36 56",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.60 -0.80 1.55 -0.71 -0.71 0.00",
	"VER 24 23 64 63",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.25 -0.95 1.55 -0.20 -0.98 0.00",
	"VER 23 12 65 64",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.77 -0.24 1.87 -0.92 0.40 0.00",
	"VER 22 25 15",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.75 -0.50 1.55 -0.97 -0.24 0.00",
	"VER 25 24 63 15",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.70 0.05 -6.05 1.00 0.00 0.00",
	"VER 44 51 48 52",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.07 -0.13 -2.03 -0.09 -0.99 -0.05",
	"VER 15 56 36",
	"END",
	"FAC",
	"COL 11403",
	"NOR -1.25 -0.08 -0.65 -0.45 -0.89 0.00",
	"VER 15 32 33 36",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.90 -0.10 -4.20 0.00 -1.00 0.00",
	"VER 50 52 56",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.83 -0.17 -2.37 -0.28 -0.96 -0.04",
	"VER 52 56 15",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.60 -0.80 -2.15 -0.71 -0.71 0.00",
	"VER 63 64 67 66",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.25 -0.95 -2.15 -0.20 -0.98 0.00",
	"VER 64 65 68 67",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.73 -0.37 -3.07 -1.00 0.00 -0.02",
	"VER 52 66 15",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.73 -0.57 -1.23 -0.97 -0.24 0.00",
	"VER 15 63 66",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.63 -0.20 -6.30 -0.93 -0.37 -0.02",
	"VER 52 44 45",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.63 -0.43 -5.60 -1.00 0.00 -0.09",
	"VER 45 52 66",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.40 -0.63 -6.30 -0.37 -0.92 -0.12",
	"VER 66 46 45",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.40 -0.77 -5.60 -0.69 -0.69 -0.23",
	"VER 46 66 67",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.17 -0.87 -5.60 -0.19 -0.97 -0.14",
	"VER 67 68 46",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.00 1.56 -6.09 0.00 0.00 0.00",
	"VER 31 69 70 71 72 73 42 53",
	"END",
	"FAC",
	"COL 11403",
	"NOR -2.03 -0.22 -6.87 0.00 0.00 0.00",
	"VER 47 91 90 74 77",
	"END",
	"FAC",
	"COL 11403",
	"NOR -5.00 0.00 -4.00 0.00 0.00 -1.00",
	"VER 78 79 83 82",
	"END",
	"FAC",
	"COL 11403",
	"NOR -5.10 0.00 -2.80 -1.00 0.00 0.00",
	"VER 79 80 84 83",
	"END",
	"FAC",
	"COL 11403",
	"NOR -5.00 0.00 -2.20 0.99 0.00 0.16",
	"VER 80 81 85 84",
	"END",
	"FAC",
	"COL 11403",
	"NOR -4.90 0.00 -3.40 1.00 0.00 0.00",
	"VER 81 78 82 85",
	"END",
	"FAC",
	"COL 11403",
	"NOR -5.00 0.10 -3.10 0.00 1.00 0.00",
	"VER 78 79 80 81",
	"END",
	"FAC",
	"COL 11403",
	"NOR -5.00 -0.10 -3.10 0.00 -1.00 0.00",
	"VER 82 83 84 85",
	"END",
	"FAC",
	"COL 11403",
	"NOR -0.64 -1.10 -4.32 0.00 0.00 0.00",
	"VER 86 88 89 87",
	"END",
	"FAC",
	"BRI",
	"COL 5092",
	"NOR -0.17 -0.00 -7.70 0.00 0.00 -1.00",
	"VER 37 38 39 40 41",
	"END",
	"FAC",
	"COL 14700",
	"NOR -1.90 -0.20 -6.00 0.00 0.00 0.00",
	"VER 91 76 75 90",
	"END",
	"FAC",
	"COL 14700",
	"NOR 0.17 0.47 4.56 0.40 0.91 0.07",
	"VER 2 4 98 99",
	"END",
	"FAC",
	"COL 14700",
	"NOR 0.45 0.20 4.56 0.89 0.44 0.07",
	"VER 92 93 98 99",
	"END",
	"FAC",
	"COL 14701",
	"NOR 0.58 0.23 3.37 0.89 0.45 0.06",
	"VER 93 94 97 98",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.68 0.36 1.61 0.88 0.47 0.03",
	"VER 94 95 100 97",
	"END",
	"FAC",
	"COL 14700",
	"NOR 0.45 -0.18 4.55 0.88 -0.47 0.00",
	"VER 92 93 101 102",
	"END",
	"FAC",
	"COL 14700",
	"NOR 0.18 -0.38 4.55 0.13 -0.99 0.00",
	"VER 1 3 101 102",
	"END",
	"FAC",
	"COL 14701",
	"NOR 0.63 -0.12 3.35 0.87 -0.50 0.00",
	"VER 93 94 103 101",
	"END",
	"FAC",
	"COL 14701",
	"NOR 0.28 -0.27 3.35 0.12 -0.99 0.00",
	"VER 3 11 103 101",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.77 -0.14 1.87 0.83 -0.55 0.08",
	"VER 94 103 96",
	"END",
	"FAC",
	"COL 2114",
	"NOR 0.45 -0.54 2.50 0.00 0.00 1.00",
	"VER 11 103 106 105 104 12",
	"END",
	"FAC",
	"COL 9513",
	"NOR 0.11 0.25 5.60 0.38 0.89 0.25",
	"VER 2 99 0",
	"END",
	"FAC",
	"COL 9513",
	"NOR 0.27 0.09 5.60 0.87 0.43 0.23",
	"VER 92 99 0",
	"END",
	"FAC",
	"COL 9513",
	"NOR 0.27 -0.15 5.60 0.87 -0.46 0.19",
	"VER 0 92 102",
	"END",
	"FAC",
	"COL 9513",
	"NOR 0.10 -0.29 5.60 0.13 -0.98 0.14",
	"VER 0 102 1",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.39 0.74 3.04 0.97 0.10 0.22",
	"VER 107 97 109 110",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.18 1.09 3.15 0.55 0.84 0.06",
	"VER 5 6 109 110",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.43 0.76 2.16 0.98 0.17 -0.09",
	"VER 97 108 111 109",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.20 1.08 2.25 0.55 0.83 0.10",
	"VER 6 7 111 109",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.09 0.88 3.77 0.57 0.63 0.52",
	"VER 5 110 4",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.20 0.68 3.71 0.92 0.04 0.38",
	"VER 4 110 107",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.11 0.97 1.51 0.46 0.86 -0.21",
	"VER 7 8 111",
	"END",
	"FAC",
	"COL 31",
	"NOR 0.24 0.81 1.47 0.89 0.22 -0.41",
	"VER 111 8 108",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.38 0.67 1.50 0.42 0.90 0.11",
	"VER 97 108 8 100",
	"END",
	"FAC",
	"COL 14701",
	"NOR 0.31 0.51 3.59 0.41 0.91 0.07",
	"VER 97 98 4 107",
	"END",
	"FAC",
	"COL 11403",
	"NOR 3.15 0.00 -2.65 0.00 0.00 0.00",
	"VER 113 114 115 116",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.00 0.10 1.27 0.42 0.90 0.14",
	"VER 95 112 94",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.00 -0.10 1.27 0.42 -0.90 0.14",
	"VER 94 96 112",
	"END",
	"FAC",
	"COL 5285",
	"NOR 0.19 0.44 -7.35 0.36 0.83 -0.43",
	"VER 42 37 117 120",
	"END",
	"FAC",
	"COL 5285",
	"NOR 0.45 0.18 -7.35 0.84 0.34 -0.42",
	"VER 117 118 121 120",
	"END",
	"FAC",
	"COL 5285",
	"NOR 0.45 -0.19 -7.35 0.84 -0.34 -0.42",
	"VER 118 119 122 121",
	"END",
	"FAC",
	"COL 5285",
	"NOR 0.19 -0.45 -7.35 0.34 -0.84 -0.42",
	"VER 119 41 46 122",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.00 0.07 -5.73 1.00 0.00 0.00",
	"VER 125 126 123",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.85 0.15 -6.15 0.00 0.99 -0.12",
	"VER 127 125 123 124",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.85 -0.05 -6.15 0.00 -1.00 -0.04",
	"VER 126 128 124 123",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.27 0.73 -1.40 0.09 1.00 0.00",
	"VER 54 31 132 133",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.80 0.50 -1.40 0.62 0.78 0.00",
	"VER 131 129 132 133",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.23 0.63 -3.85 0.07 1.00 0.00",
	"VER 31 53 134 132",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.66 0.46 -3.85 0.63 0.77 0.00",
	"VER 129 127 134 132",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.63 0.26 -6.30 0.92 0.38 -0.05",
	"VER 120 121 127",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.52 0.45 -5.60 0.63 0.77 0.01",
	"VER 134 127 120",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.29 0.59 -6.30 0.39 0.92 -0.01",
	"VER 42 120 134",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.12 0.62 -5.60 0.05 1.00 0.05",
	"VER 42 134 53",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.23 0.15 -1.67 0.71 0.71 0.00",
	"VER 129 131 113 116",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.13 0.10 -3.53 0.00 0.97 -0.26",
	"VER 129 135 116",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.00 0.12 -3.87 1.00 0.00 0.00",
	"VER 129 136 125 126 130 135",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.90 0.30 -4.20 0.00 1.00 0.00",
	"VER 129 125 127",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.20 0.78 0.59 0.35 0.93 -0.09",
	"VER 8 54 100",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.37 0.73 0.26 0.09 0.99 0.12",
	"VER 100 54 133",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.64 0.57 0.46 0.86 0.52 0.00",
	"VER 100 133 95",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.81 0.45 0.20 0.59 0.75 0.29",
	"VER 95 133 131",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.10 0.20 0.43 0.41 0.89 0.20",
	"VER 131 112 95",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.30 0.10 0.23 0.71 0.71 0.00",
	"VER 112 131 113",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.13 -0.03 -3.53 0.00 -1.00 -0.09",
	"VER 135 116 130",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.60 -0.80 1.55 0.71 -0.71 0.00",
	"VER 105 104 138 137",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.25 -0.95 1.55 0.20 -0.98 0.00",
	"VER 104 12 65 138",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.77 -0.24 1.87 0.92 0.40 0.00",
	"VER 103 106 96",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.75 -0.50 1.55 0.97 -0.24 0.00",
	"VER 106 105 137 96",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.70 0.05 -6.05 -1.00 0.00 0.00",
	"VER 121 127 124 128",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.07 -0.13 -2.03 0.09 -0.99 -0.05",
	"VER 96 130 116",
	"END",
	"FAC",
	"COL 11403",
	"NOR 1.25 -0.08 -0.65 0.45 -0.89 0.00",
	"VER 96 112 113 116",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.90 -0.10 -4.20 0.00 -1.00 0.00",
	"VER 126 128 130",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.83 -0.17 -2.37 0.28 -0.96 -0.04",
	"VER 128 130 96",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.60 -0.80 -2.15 0.71 -0.71 0.00",
	"VER 137 138 140 139",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.25 -0.95 -2.15 0.20 -0.98 0.00",
	"VER 138 65 68 140",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.73 -0.37 -3.07 1.00 0.00 -0.02",
	"VER 128 139 96",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.73 -0.57 -1.23 0.97 -0.24 0.00",
	"VER 96 137 139",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.63 -0.20 -6.30 0.93 -0.37 -0.02",
	"VER 128 121 122",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.63 -0.43 -5.60 1.00 0.00 -0.09",
	"VER 122 128 139",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.40 -0.63 -6.30 0.37 -0.92 -0.12",
	"VER 139 46 122",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.40 -0.77 -5.60 0.69 -0.69 -0.23",
	"VER 46 139 140",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.17 -0.87 -5.60 0.19 -0.97 -0.14",
	"VER 140 68 46",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.00 1.56 -6.09 0.00 0.00 0.00",
	"VER 31 69 70 71 72 73 42 53",
	"END",
	"FAC",
	"COL 11403",
	"NOR 2.03 -0.22 -6.87 0.00 0.00 0.00",
	"VER 123 158 157 141 144",
	"END",
	"FAC",
	"COL 11403",
	"NOR 5.00 0.00 -4.00 0.00 0.00 -1.00",
	"VER 145 146 150 149",
	"END",
	"FAC",
	"COL 11403",
	"NOR 5.10 0.00 -2.80 1.00 0.00 0.00",
	"VER 146 147 151 150",
	"END",
	"FAC",
	"COL 11403",
	"NOR 5.00 0.00 -2.20 -0.99 0.00 0.16",
	"VER 147 148 152 151",
	"END",
	"FAC",
	"COL 11403",
	"NOR 4.90 0.00 -3.40 -1.00 0.00 0.00",
	"VER 148 145 149 152",
	"END",
	"FAC",
	"COL 11403",
	"NOR 5.00 0.10 -3.10 0.00 1.00 0.00",
	"VER 145 146 147 148",
	"END",
	"FAC",
	"COL 11403",
	"NOR 5.00 -0.10 -3.10 0.00 -1.00 0.00",
	"VER 149 150 151 152",
	"END",
	"FAC",
	"COL 11403",
	"NOR 0.64 -1.10 -4.32 0.00 0.00 0.00",
	"VER 153 155 156 154",
	"END",
	"FAC",
	"BRI",
	"COL 5092",
	"NOR 0.17 -0.00 -7.70 0.00 0.00 -1.00",
	"VER 37 117 118 119 41",
	"END",
	"FAC",
	"COL 14700",
	"NOR 1.90 -0.20 -6.00 0.00 0.00 0.00",
	"VER 158 143 142 157",
	"END",
	"BAS 0.00 -100.00 250.00 0 16384 -32768",
	"BAS -38.00 43.00 421.00 -16644 4378 636",
	"BAS -59.00 65.00 77.00 -12545 3907 -30344",
	"BAS 0.00 0.00 -770.00 0 16384 0",
	"BAS 0.00 0.00 -700.00 0 16384 0",
	"BAS -100.00 0.00 -740.00 0 -401 -30775",
	"BAS 0.00 0.00 -230.00 0 0 -16384",
	"BAS -50.00 -90.00 -490.00 0 0 24576",
	"BAS -58.00 -83.00 -360.00 -32768 0 -13886",
	"END",
	NULL
};

void BiFalcon2G(BIPOSATT *pos)
{
	static int first=BI_ON;
	static BISRF srf;

	if(first==BI_ON)
	{
		BiLoadSrfFromString(&srf,F16);
		BiScaleSrf(&srf,0.1F);
		first=BI_OFF;
	}

	BiInsSrf(&srf,pos);
}