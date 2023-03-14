/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nosterme <nosterme@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 12:31:47 by nosterme          #+#    #+#             */
/*   Updated: 2023/03/08 17:59:01 by nosterme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Config_HPP
# define Config_HPP

# include <iostream>
# include <fstream>
# include <cstdlib>
# include <cctype>

# include "utils.hpp"
# include "Server.hpp"

namespace web
{
	class	Config
	{
		public:
			Config(char const * filename);
			~Config(void);

			int						parse(void);
			bool					getValidation() const;
			void					setValidation(bool status);
			std::vector<params_t>	servers;	

			void					run(void);

		private:
			bool 					validation;
			std::string				_file;

			// canonical class form
			Config(void);
			Config(Config const & other);
			Config &					operator=(Config const & rhs);
			void						parse_config_file(void);
			std::vector<std::string>	get_confdata();
			int							count_servers(const std::vector<std::string> & confdata);
			int							find_close_symbol( const std::vector<std::string> & confdata,\
														   size_t* cursor, int level );
			void						fill_routes( std::vector<std::string> & data, size_t i,\
												     size_t* cursor );
			void						fill_host_value(std::string line, size_t i, size_t* cursor);
			void						skipe_spaces(const std::string & str, size_t* cursor);
			void						skipe_empty_line( const std::vector<std::string> & confdata,\
														  size_t* cursor );
			std::string					get_inline_value(const std::string & line, size_t* cursor);
			void						fill_port_value( std::string line, size_t i, size_t* cursor);
			std::string					get_route_name( std::vector<std::string> & data, size_t i, \
												 size_t* cursor );
			void						fill_server_name(std::string line, size_t i, size_t* cursor);
			std::vector<std::string>	split_by_space_or_tab(std::string str);
			void						fill_errors_pages(std::string line, size_t i, size_t* cursor);
			void						fill_client_max_body_size( std::string line, size_t i,\
																   size_t* cursor );
			bool						is_empty(const std::string & str);
			bool						is_route_well_formated( std::vector<std::string> & data,\
														        size_t i, size_t* cursor );
			std::vector<std::string>  	get_methods(std::string & str, size_t *cursor);
			void						set_dir_listing_options( std::string & str, route_t* route,\
													             size_t* cursor );
			void						set_root(std::string & str, route_t* route, size_t* cursor);
			void						set_index(std::string & str, route_t* route, size_t* cursor);
			void						set_upload(std::string & str, route_t* route, size_t* cursor);
			void						set_redirect( std::string & str, route_t* route,\
													  size_t* cursor );
			void						set_cgi_ext( std::string & str, route_t* route,\
													 size_t* cursor );
			void						set_cgi_pass( std::string & str, route_t* route,\
													  size_t* cursor );
			bool						is_valid_server_nb(int nb_servers);
			void						fill_route_params( std::vector<std::string> & data, size_t i,\
														   size_t* cursor, std::string & route_name );
			bool						is_valid_host(std::string host);
			bool						is_valid_ip(std::vector<std::string> & all_octes);
			bool						is_valid_errors_code(std::vector<std::string> errors_code);
			


	};
}

#endif
