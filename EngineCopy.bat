xcopy /s /y  /exclude:exclude_list.txt ".\01. Project\Engine\*.h" ".\03. External\include\Engine"
xcopy /s /y  /exclude:exclude_list.txt ".\01. Project\Engine\*.hpp" ".\03. External\include\Engine"
xcopy /s /y  /exclude:exclude_list.txt ".\01. Project\Engine\*.inl" ".\03. External\include\Engine"
xcopy /s /y  /exclude:exclude_list.txt ".\01. Project\Engine\*.fx" ".\02. OutPut\bin\content\shader"