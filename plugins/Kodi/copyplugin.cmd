@echo off
copy .\%1*.dll ..\.. > NUL
copy .\%1*.dll ..\..\%1 > NUL