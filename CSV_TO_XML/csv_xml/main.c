
  #include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>



void search_all_file ( char *path )
{
  DIR              *pDir ;
  struct dirent    *ent  ;
  int               i=0  ;
  char              childpath[512];

  pDir=opendir ( path );
  memset ( childpath,0,sizeof ( childpath ) );


  while ( ( ent=readdir ( pDir ) ) !=NULL )
    {

      if ( ent->d_type & DT_DIR )
        {

          if ( strcmp ( ent->d_name,"." ) ==0 || strcmp ( ent->d_name,".." ) ==0 )
            continue;

          sprintf ( childpath,"%s/%s",path,ent->d_name );
          printf ( "path:%s/n",childpath );

          search_all_file ( childpath );

        }
      else
        {
        printf("ent->d_name %s\n",ent->d_name);
        
        }
    }

}

int main(int argc,char *argv[])  
{  
        search_all_file(argv[1]);  
        return 0;  
}