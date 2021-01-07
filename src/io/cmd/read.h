//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_READ_H
#define FICTION_READ_H


#include "network_reader.h"
#include <alice/alice.hpp>
#include <string>


namespace alice
{
    /**
     * Shorthand for all read commands. Chooses the proper function by the file ending.
     *
     * Parses verilog files currently. Gate level netlists are supported. The lorina verilog parser is used. At the
     * moment, only 2-input gates + 3-input MAJ in assign notation are supported. Using ABC, those can be generated by
     * the commands
     *
     * read <file>
     * strash
     * write <file>.v
     *
     * For more information see: https://github.com/marcelwa/lorina
     */
    class read_command : public command
    {
    public:
        /**
         * Standard constructor. Adds descriptive information, options, and flags.
         *
         * @param env alice::environment that specifies stores etc.
         */
        explicit read_command(const environment::ptr& env)
                :
                command(env,
                        "Reads one Verilog or AIGER file or a directory of those and creates logic network objects "
                        "which will be put into the respective store. In a directory, only files with extension "
                        "'.v' and '.aig' are considered.")
        {
            add_option("filename", filename,
                       "Verilog or AIGER filename or directory")->required();
            add_flag("--sort,-s", sort,
                     "Sort networks in given directory by vertex count prior to storing them");
        }

    protected:
        /**
         * Function to perform the read call. Reads Verilog and creates a logic_network.
         */
        void execute() override
        {
            try
            {
                network_reader reader{filename, env->out()};

                for (const auto& ln : reader.get_networks(sort))
                    store<logic_network_ptr>().extend() = ln;
            }
            catch (...)
            {
                env->out() << "[e] no networks were read" << std::endl;
            }

            // reset flags, necessary for some reason... alice bug?
            sort = false;
        }

    private:
        /**
         * Verilog filename.
         */
        std::string filename;
        /**
         * Flag to indicate that files should be sorted by file size.
         */
        bool sort = false;
    };

    ALICE_ADD_COMMAND(read, "I/O")
}


#endif //FICTION_READ_H
