﻿/* Portions Copyright 2019 Xuesong Zhou and Peiheng Li
 *
 * If you help write or modify the code, please also list your names here.
 * The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL
 * and further prevent a violation of the GPL.
 *
 * More about "How to use GNU licenses for your own software"
 * http://www.gnu.org/licenses/gpl-howto.html
 */

#ifdef _WIN32
#include "pch.h"
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include "config.h"
#include "utils.h"

int main()
{
    // reset all the log files to defult 0: not output; if want to output these logs set to 1
    dtalog.output() << "DTALite Log" << std::fixed << std::setw(12) << '\n';
    dtalog.debug_level() = 0;
    dtalog.log_sig() = 0;
    dtalog.log_odme() = 0;
    dtalog.log_path() = 0;
    dtalog.log_dta() = 0;
    dtalog.log_ue() = 0;

    int iteration_number = 20;
    int column_updating_iterations = 40;
    int ODME_iterations = 20;
    int VDF_type = 0;
	int number_of_memory_blocks = 8;

    int signal_updating_output = 0;
    // generate link performance and agent file
    int assignment_mode = 1;
    bool flag_default = false;
    int default_volume = 1;

    CCSVParser parser_settings;
    parser_settings.IsFirstLineHeader = false;

    if (parser_settings.OpenCSVFile("settings.csv", false))
    {
        while (parser_settings.ReadRecord_Section())
        {
            if (parser_settings.SectionName == "[assignment]")
            {
                std::string assignment_mode_str;
                parser_settings.GetValueByFieldName("number_of_iterations", iteration_number, true, true);
                parser_settings.GetValueByFieldName("assignment_mode", assignment_mode_str);

                // these are the assignment modes
                // two usually methods are ue (user equilibrium) and dta (dynamic traffic assignment)
                // the main difference of these two methods are different output in link_performance.csv
                // for basic uses set assignment mode to 'ue'
                // for more detailed link performances (one minute) set 'dta'
                if (assignment_mode_str == "lue")
                    assignment_mode = 0;
                else if (assignment_mode_str == "ue")
                    assignment_mode = 1;
                else if (assignment_mode_str == "dta")
                    assignment_mode = 2;
                else if (assignment_mode_str == "odme")
                    assignment_mode = 3;
                else if (assignment_mode_str == "grid2zone")  
                    assignment_mode = 20;
                else if (assignment_mode_str == "zone2access")  //access link
                    assignment_mode = 21;
                else if (assignment_mode_str == "zone2demand")  
                    assignment_mode = 22;
                else if (assignment_mode_str == "cbi")  //congestion bottleneck identification
                    assignment_mode = 11;
                else if (assignment_mode_str == "cbsa")  //congestion bottleneck sensitivity
                    assignment_mode = 12;
                else
                {
                    dtalog.output() << "assignment_mode " << assignment_mode_str.c_str() << " in settings.csv is invalid." << std::endl;
                    g_program_stop();
                }

                // iteration number of reassignment
                parser_settings.GetValueByFieldName("column_updating_iterations", column_updating_iterations, true, true);
                parser_settings.GetValueByFieldName("ODME_iterations", ODME_iterations, false, false);
                parser_settings.GetValueByFieldName("VDF_type", VDF_type, false, false);

                // the start interation of generating signals, if there is no signals set this number larger than the iteration number
                parser_settings.GetValueByFieldName("number_of_memory_blocks", number_of_memory_blocks, false, false);
				dtalog.output() << "number_of_memory_blocks = " << number_of_memory_blocks << " in settings.csv." << std::endl;



                // just one record
                break;
            }

            if (parser_settings.SectionName == "[log]")
            {
                parser_settings.GetValueByFieldName("sig", dtalog.log_sig(), false);
                parser_settings.GetValueByFieldName("odme", dtalog.log_odme(), false);
                parser_settings.GetValueByFieldName("path", dtalog.log_path(), false);
                parser_settings.GetValueByFieldName("ue", dtalog.log_ue(), false);

                // just one record
                break;
            }
        }
    }
    // obtain initial flow values
    network_assignment(assignment_mode, VDF_type, iteration_number, column_updating_iterations, ODME_iterations, number_of_memory_blocks);

    return 0;
}