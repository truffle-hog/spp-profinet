for i in $(find . -name '*_template.c')
do
    #rename -v 's/(.*)(test)_([a-zA-Z_]*)/$1$3_$2/' $i
    #rename -v 's/([a-zA-Z_]*)(test)/$1$2_template/' $i
    #rename -vn 's/(.*)_template.*/$1/' $i
    rename -v 's/_template//' $i
done
