
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
int main()
{
    char GETbuff[256]="TZ";
    char SETbuff[256]="export TZ=UTC-05:00";
    char tmp[100] = { 0 };

    printf ( "getenv -- %s \n"  , getenv ( GETbuff ) );

// //     setenv ( "TZ", "UTC-01:00",1 );
//     putenv ( "TZ=UTC-14:00" );
//     //  system(" export TZ=UTC-04:00");
// //         printf ( " \n system -  export TZ=UTC-04:00 ---\n"  );
// //    // system("source time_utc.sh");
// 
//     tzset();
// 
//     printf ( "gettime locatime: %s \n"  , getenv ( GETbuff ) );
//         tzset();
// 	    tzset();
// 	        tzset();
// 		    tzset();
// 		    
// 		       printf ( "gettime locatime: %s \n"  , getenv ( GETbuff ) );
		       
		       system("date ");
		       
		         int16_t bit_map[32] = {0};
	memset(bit_map ,-1 ,sizeof(bit_map ));
        int i = 0;
	
	for ( i = 0 ; i < 32 ; i++)
	{
	  printf("bit_map %0d \n",bit_map[i]);
	}
    return 0;
}


