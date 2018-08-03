FUNCTION $sc_$cpu_is_app_loaded (appname)

local app_loaded = FALSE

write "inside _is_app_loaded function looking for appname = ", appname

;; NOTE: Will not work if the CFE_ES_MAX_APPLICATIONS configuration parameter
;;       is not 32 and the app being searched for is not in the 1st 32 apps
FOR i = 1 to 32 DO
   IF ($sc_$cpu_es_ale[i].es_al_appname = appname) THEN
   app_loaded = TRUE
   WRITE "Application '", appname, "' is already loaded as taskid ", $SC_$CPU_ES_ALE[i].es_al_taskid
   ENDIF
   CONTINUE IF app_loaded = FALSE
ENDDO

return app_loaded

ENDFUNC
