//
//  main.cpp
//  RandomForest++
//
//  Created by Brian Putnam on 10/3/14.
//  Copyright (c) 2014 Brian Putnam. All rights reserved.
//

#include <iostream>
#include "libs/libcsv_parser.h"

int main(int argc, const char * argv[]) {
    
    
    csv_parser file_parser;
    file_parser.set_skip_lines(1);
    file_parser.init("data/training.csv");
    file_parser.set_enclosed_char('"', ENCLOSURE_OPTIONAL);
    file_parser.set_field_term_char(',');
    file_parser.set_line_term_char('\n');
    
    int count = 0;
    while (file_parser.has_more_rows()) {
        csv_row row = file_parser.get_row();
        
        for (int i=0; i<row.size(); i++) {
            cout << row[i] << "\t";
        }
        cout << endl;
        
        if (count > 5) {
            exit(0);
        }
        count++;
    }
    
    return 0;
}
