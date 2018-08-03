Readme file for the gen_app_code script.

This script will generate the base code, including the table definitions, for the new applications listed in the command.

1. Got to your $CFS_MISSION/apps directory
2. Execute the Python script located in $CFS_MISSION/tools/gen_app_code/gen_app_code.py
3. Follow the usage description to generate the base code for your new applications.  
   
   Example: > $CFS_MISSION/tools/gen_app_code/gen_app_code.py MISSION PrjFooBar OWNER "Jane Doe" OUTDIR . APPS newApp1 newApp2 TBLS iload
   
Note: Pay close attention to how you name your applications at this step since the specified application names will be used as part of the file 
names, function names, variable names and macro names through out the code from here on.

Follow the directions in the cFS Deployment Guide to integrate the new application.  Note the cFS Deployment Guide step "Place Application In $APP_DIR" 
has already been completed by running this script in the $CFS_MISSION/apps directory.
