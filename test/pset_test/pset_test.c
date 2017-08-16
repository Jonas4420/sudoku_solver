#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "preemptive_set.h"

/* gcc -I ../include -c pset_test.c */
/* gcc -o pset_test pset_test.o preemptive_set.o */

void
display_result (bool test)
{
  if (test)
    fprintf (stdout, "(passed)\n");
  else
    fprintf (stdout, "(failed!)\n");
}

int
main (void)
{
  pset_t p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
  char str[MAX_COLORS + 1];
  char str2[MAX_COLORS + 1];
  char str3[MAX_COLORS + 1];

  /* singleton: "D" */
  p0 = char2pset ('D');

  /* pset: "137" */
  p1 = pset_set (pset_set (char2pset ('7'), '3'), '1');

  /* pset: "137AO" */
  p2 = pset_set (pset_set (p1, 'O'), 'A');

  /* not valid char */
  p3 = pset_set (p1, '#');

  /* Testing pset2str */
  /********************/
  fputs ("pset2str\n" "========\n", stdout);

  pset2str (str, p0);
  printf ("pset2str (\"D\"): %s ", str);
  display_result (!strcmp (str, "D"));

  pset2str (str, p1);
  printf ("pset2str (\"137\"): %s ", str);
  display_result (!strcmp (str, "137"));

  pset2str (str, p2);
  printf ("pset2str (\"137AO\"): %s ", str);
  display_result (!strcmp (str, "137AO"));

  pset2str (str, p3);
  printf ("pset2str (\"137#\"): %s ", str);
  display_result (!strcmp (str, "137"));

  fputs ("\n", stdout);


  /* Testing pset_full */
  /*********************/
  fputs ("pset_full\n" "=========\n", stdout);

  p1 = pset_full (1);
  pset2str (str, p1);
  printf ("pset_full (1): %s ", str);
  display_result (!strcmp (str, "1"));

  p2 = pset_full (4);
  pset2str (str, p2);
  printf ("pset_full (4): %s ", str);
  display_result (!strcmp (str, "1234"));

  p3 = pset_full (9);
  pset2str (str, p3);
  printf ("pset_full (9): %s ", str);
  display_result (!strcmp (str, "123456789"));

  p4 = pset_full (16);
  pset2str (str, p4);
  printf ("pset_full (16): %s ", str);
  display_result (!strcmp (str, "123456789ABCDEFG"));

  p5 = pset_full (25);
  pset2str (str, p5);
  printf ("pset_full (25): %s ", str);
  display_result (!strcmp (str, "123456789ABCDEFGHIJKLMNOP"));

  p6 = pset_full (36);
  pset2str (str, p6);
  printf ("pset_full (36): %s ", str);
  display_result (!strcmp (str, "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZa"));

  p7 = pset_full (49);
  pset2str (str, p7);
  printf ("pset_full (49): %s ", str);
  display_result (!strcmp
		  (str, "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"));

  p8 = pset_full (64);
  pset2str (str, p8);
  printf ("pset_full (64): %s ", str);
  display_result (!strcmp (str,
			   "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*"));

  /* has not been specified ! */
  p9 = pset_full (17);
  pset2str (str, p9);
  printf ("pset_full (17): %s ", str);
  display_result (!strcmp (str, "123456789ABCDEFGH"));

  /* has not been specified ! */
  p10 = pset_full (73);
  pset2str (str, p10);
  printf ("pset_full (73): %s ", str);
  display_result (!strcmp (str,
			   "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*"));

  fputs ("\n", stdout);


  /* Testing pset_empty */
  /**********************/
  fputs ("pset_empty\n" "==========\n", stdout);

  p0 = pset_empty ();
  pset2str (str, p0);
  printf ("pset_empty (): \"%s\" ", str);
  display_result (!strcmp (str, ""));

  fputs ("\n", stdout);


  /* Testing pset_set */
  /********************/
  fputs ("pset_set\n" "========\n", stdout);

  p0 = char2pset ('A');
  pset2str (str, p0);
  p0 = pset_set (p0, 'D');
  pset2str (str2, p0);
  printf ("pset_set (\"%s\", 'D'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "AD"));

  pset2str (str, p1);
  p1 = pset_set (p1, '5');
  pset2str (str2, p1);
  printf ("pset_set (\"%s\", '5'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "15"));

  pset2str (str, p2);
  p2 = pset_set (p2, 'Z');
  pset2str (str2, p2);
  printf ("pset_set (\"%s\", 'Z'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "1234Z"));

  /* has not been specified ! */
  pset2str (str, p3);
  p3 = pset_set (p3, '#');
  pset2str (str2, p3);
  printf ("pset_set (\"%s\", '#'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "123456789"));

  fputs ("\n", stdout);


  /* Testing pset_discard */
  /************************/
  fputs ("pset_discard\n" "============\n", stdout);

  /* pset: "137" */
  p0 = pset_set (pset_set (char2pset ('1'), '3'), '7');
  /* pset: "13" */
  p1 = pset_set (char2pset ('1'), '3');
  /* pset: "37" */
  p2 = pset_set (char2pset ('3'), '7');

  p3 = p0;
  pset2str (str, p3);
  p3 = pset_discard (p3, '7');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", '7'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "13"));

  p3 = p0;
  pset2str (str, p3);
  p3 = pset_discard (p3, '1');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", '1'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "37"));

  p3 = p0;
  pset2str (str, p3);
  p3 = pset_discard (p3, 'Z');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", 'Z'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "137"));

  pset2str (str, p3);
  p3 = pset_discard (p3, '1');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", '1'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "37"));

  p3 = pset_set (p3, '1');
  pset2str (str, p3);
  p3 = pset_discard (p3, '#');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", '#'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, "137"));

  p3 = pset_empty ();
  pset2str (str, p3);
  p3 = pset_discard (p3, '4');
  pset2str (str2, p3);
  printf ("pset_discard (\"%s\", '4'): \"%s\" ", str, str2);
  display_result (!strcmp (str2, ""));

  fputs ("\n", stdout);


  /* Testing pset_negate */
  /***********************/
  fputs ("pset_negate\n" "===========\n", stdout);

  p0 = pset_full (9);
  p1 = pset_full (25);

  /* pset: "137F" */
  p2 = pset_set (pset_set (pset_set (char2pset ('1'), '3'), '7'), 'F');
  /* pset: "245689ABCDEGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*" */
  p3 = pset_negate (p2);

  p4 = pset_negate (p0);
  pset2str (str, p4);
  printf ("pset_negate (\"123456789\"): %s ", str);
  display_result (!strcmp (str,
			   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*"));

  p4 = pset_negate (p1);
  pset2str (str, p4);
  printf ("pset_negate (\"123456789ABCDEFGHIJKLMNOP\"): '%s' ", str);
  display_result (!strcmp (str, "QRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*"));

  p4 = pset_negate (p2);
  pset2str (str, p4);
  printf ("pset_negate (\"137F\"): %s ", str);
  display_result (!strcmp (str,
			   "245689ABCDEGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@&*"));

  p6 = pset_full (33);
  p4 = pset_negate (p6);
  pset2str (str, p4);
  printf ("pset_negate (pset_full (33)): '%s' ", str);
  display_result (!strcmp (str, "YZabcdefghijklmnopqrstuvwxyz@&*"));

  p6 = pset_full (64);
  p4 = pset_negate (p6);
  pset2str (str, p4);
  printf ("pset_negate (pset_full (64)): '%s' ", str);
  display_result (!strcmp (str, ""));

  fputs ("\n", stdout);


  /* Testing pset_and */
  /********************/
  fputs ("pset_and\n" "========\n", stdout);

  /* "12357F" */
  p1 = pset_set (pset_set (char2pset ('1'), '2'), '3');
  p1 = pset_set (pset_set (pset_set (p1, '7'), 'F'), '5');

  /* "267AF" */
  p2 = pset_set (pset_set (char2pset ('2'), '6'), '7');
  p2 = pset_set (pset_set (p2, 'A'), 'F');

  /* "27F" */
  p3 = pset_set (pset_set (char2pset ('2'), '7'), 'F');

  pset2str (str, p1);
  pset2str (str2, p2);
  p4 = pset_and (p1, p2);
  pset2str (str3, p4);
  printf ("pset_and (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "27F"));

  /* "Zcd" */
  p3 = pset_set (pset_set (char2pset ('Z'), 'c'), 'd');

  pset2str (str, p1);
  pset2str (str2, p3);
  p4 = pset_and (p1, p3);
  pset2str (str3, p4);
  printf ("pset_and (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, ""));

  pset2str (str, p1);
  pset2str (str2, pset_empty ());
  p4 = pset_and (p1, pset_empty ());
  pset2str (str3, p4);
  printf ("pset_and (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, ""));

  pset2str (str, pset_empty ());
  pset2str (str2, pset_empty ());
  p4 = pset_and (pset_empty (), pset_empty ());
  pset2str (str3, p4);
  printf ("pset_and (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, ""));

  fputs ("\n", stdout);

  /* Testing pset_or */
  /*******************/
  fputs ("pset_or\n" "=======\n", stdout);

  /* "12357F" */
  p1 = pset_set (pset_set (char2pset ('1'), '2'), '3');
  p1 = pset_set (pset_set (pset_set (p1, '7'), 'F'), '5');

  /* "267AF" */
  p2 = pset_set (pset_set (char2pset ('2'), '6'), '7');
  p2 = pset_set (pset_set (p2, 'A'), 'F');

  /* "27F" */
  p3 = pset_set (pset_set (char2pset ('2'), '7'), 'F');

  pset2str (str, p1);
  pset2str (str2, p2);
  p4 = pset_or (p1, p2);
  pset2str (str3, p4);
  printf ("pset_or (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "123567AF"));

  /* "Zcd" */
  p3 = pset_set (pset_set (char2pset ('Z'), 'c'), 'd');

  pset2str (str, p1);
  pset2str (str2, p3);
  p4 = pset_or (p1, p3);
  pset2str (str3, p4);
  printf ("pset_or (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "12357FZcd"));

  pset2str (str, p1);
  pset2str (str2, pset_empty ());
  p4 = pset_or (p1, pset_empty ());
  pset2str (str3, p4);
  printf ("pset_or (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "12357F"));

  pset2str (str, pset_empty ());
  pset2str (str2, pset_empty ());
  p4 = pset_or (pset_empty (), pset_empty ());
  pset2str (str3, p4);
  printf ("pset_or (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, ""));

  fputs ("\n", stdout);


  /* Testing pset_xor */
  /********************/
  fputs ("pset_xor\n" "========\n", stdout);

  /* "12357F" */
  p1 = pset_set (pset_set (char2pset ('1'), '2'), '3');
  p1 = pset_set (pset_set (pset_set (p1, '7'), 'F'), '5');

  /* "267AF" */
  p2 = pset_set (pset_set (char2pset ('2'), '6'), '7');
  p2 = pset_set (pset_set (p2, 'A'), 'F');

  /* "27F" */
  p3 = pset_set (pset_set (char2pset ('2'), '7'), 'F');

  pset2str (str, p1);
  pset2str (str2, p2);
  p4 = pset_xor (p1, p2);
  pset2str (str3, p4);
  printf ("pset_xor (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "1356A"));

  /* "Zcd" */
  p3 = pset_set (pset_set (char2pset ('Z'), 'c'), 'd');

  pset2str (str, p1);
  pset2str (str2, p3);
  p4 = pset_xor (p1, p3);
  pset2str (str3, p4);
  printf ("pset_xor (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "12357FZcd"));

  pset2str (str, p1);
  pset2str (str2, pset_empty ());
  p4 = pset_xor (p1, pset_empty ());
  pset2str (str3, p4);
  printf ("pset_xor (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, "12357F"));

  pset2str (str, pset_empty ());
  pset2str (str2, pset_empty ());
  p4 = pset_xor (pset_empty (), pset_empty ());
  pset2str (str3, p4);
  printf ("pset_xor (\"%s\",\"%s\"): \"%s\" ", str, str2, str3);
  display_result (!strcmp (str3, ""));

  fputs ("\n", stdout);


  /* Testing pset_is_singleton */
  /*****************************/
  fputs ("pset_is_singleton\n" "=================\n", stdout);

  pset2str (str, p1);
  printf ("pset_is_singleton (\"%s\"): %s ", str,
	  (pset_is_singleton (p1) ? "true" : "false"));
  display_result ((pset_is_singleton (p1) == false));

  p4 = char2pset ('F');
  pset2str (str, p4);
  printf ("pset_is_singleton (\"%s\"): %s ", str,
	  (pset_is_singleton (p4) ? "true" : "false"));
  display_result ((pset_is_singleton (p4) == true));

  p4 = char2pset ('*');
  pset2str (str, p4);
  printf ("pset_is_singleton (\"%s\"): %s ", str,
	  (pset_is_singleton (p4) ? "true" : "false"));
  display_result ((pset_is_singleton (p4) == true));

  p4 = pset_set (char2pset ('1'), '*');
  pset2str (str, p4);
  printf ("pset_is_singleton (\"%s\"): %s ", str,
	  (pset_is_singleton (p4) ? "true" : "false"));
  display_result ((pset_is_singleton (p4) == false));

  p4 = pset_empty ();
  pset2str (str, p4);
  printf ("pset_is_singleton (\"%s\"): %s ", str,
	  (pset_is_singleton (p4) ? "true" : "false"));
  display_result ((pset_is_singleton (p4) == false));

  fputs ("\n", stdout);


  /* Testing pset_is_included */
  /****************************/
  fputs ("pset_is_included\n" "================\n", stdout);

  pset2str (str, p1);
  pset2str (str2, p2);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p1, p2) ? "true" : "false"));
  display_result ((pset_is_included (p1, p2) == false));

  pset2str (str, p3);
  pset2str (str2, p1);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p3, p1) ? "true" : "false"));
  display_result ((pset_is_included (p3, p1) == false));

  pset2str (str, p1);
  pset2str (str2, p3);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p1, p3) ? "true" : "false"));
  display_result ((pset_is_included (p1, p3) == false));

  p4 = pset_empty ();

  pset2str (str, p4);
  pset2str (str2, p1);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p4, p1) ? "true" : "false"));
  display_result ((pset_is_included (p4, p1) == true));

  p6 = pset_full (49);
  p4 = p3;

  pset2str (str, p6);
  pset2str (str2, p4);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p6, p4) ? "true" : "false"));
  display_result ((pset_is_included (p6, p4) == false));

  pset2str (str, p4);
  pset2str (str2, p6);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p4, p6) ? "true" : "false"));
  display_result ((pset_is_included (p4, p6) == true));

  pset2str (str, p3);
  pset2str (str2, p4);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p3, p4) ? "true" : "false"));
  display_result ((pset_is_included (p3, p4) == true));

  p3 = pset_empty ();
  p4 = pset_empty ();
  pset2str (str, p4);
  pset2str (str2, p3);
  printf ("pset_is_included (\"%s\", \"%s\"): %s ",
	  str, str2, (pset_is_included (p4, p3) ? "true" : "false"));
  display_result ((pset_is_included (p4, p3) == true));

  fputs ("\n", stdout);


  /* Testing pset_cardinality */
  /****************************/
  fputs ("pset_cardinality\n" "================\n", stdout);

  pset2str (str, p1);
  printf ("pset_cardinality (\"12357F\"): %d ", pset_cardinality (p1));
  display_result ((pset_cardinality (p1) == 6));

  p4 = char2pset ('F');
  pset2str (str, p4);
  printf ("pset_cardinality (\"F\"): %d ", pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 1));

  p4 = pset_empty ();
  pset2str (str, p4);
  printf ("pset_cardinality (\"\"): %d ", pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 0));

  p4 = pset_full (4);
  pset2str (str, p4);
  printf ("pset_cardinality (\"1234\"): %d ", pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 4));

  p4 = pset_full (9);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789\"): %d ", pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 9));

  p4 = pset_full (16);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789ABCDEF\"): %d ",
	  pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 16));

  p4 = pset_full (25);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789ABCDEFGHIJKLMNOP\"): %d ",
	  pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 25));

  p4 = pset_full (17);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789ABCDEFGH\"): %d ",
	  pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 17));

  p4 = pset_full (29);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789ABCDEFGHIJKLMNOPQRST\"): %d ",
	  pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 29));

  p4 = pset_full (37);
  pset2str (str, p4);
  printf ("pset_cardinality (\"123456789ABCDEFGHIJKLMNOPQRSTUVW\"): %d ",
	  pset_cardinality (p4));
  display_result ((pset_cardinality (p4) == 37));

  return EXIT_SUCCESS;
}
