//Title: LogMaster-MTA (LogMaster-MailTransferAgent)
//Author: Röper Luke, Stadt Augsburg
//Non-Release-Version: 0.4.2
//Release-Version: 1.2

/*Headers*/
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <windows.h>

/*Define*/
#define CMDLINE_PARAMETERS_COUNT_MAX 7
#define DEFAULTS_FILE_LENGTH 3
#define DEFAULTS_LINE_LENGTH 100
#define ARRAY_TABLE_WIDTH_PER_ELEMENT 14
//(ID, postfix-queue-id-1, postfix-queue-id-2, msg-id, from-mail, to-mail, amavis-scan-id, amavis-mail-id, internal-id, amavis-response, internal-pipe-response, relay-exc-response, complete?, time

/*Variables*/
std::string parameters[CMDLINE_PARAMETERS_COUNT_MAX+2]; //Holds usable values for below described parameters
std::string default_parameters[DEFAULTS_FILE_LENGTH]; //Holds Loaded values from .defaults file to be transferres into parameters[]
const std::string default_parameters_identifiers[] = {"system", "exec-mode", "source-files-path"};
//system: 0 = Windows, 1 = Linux
//exec-mode: 0 = command-line, 1 = output-to-file, 2 = debug, 3 = repair-defaults(, 4 = GUI)
//source-files-path: Path to the MTA-Logfiles (C:\\Log\... or /var/log/mail/)
std::ifstream logfile;
std::string logfileline;
int linescounter = 0;
std::ofstream logmaster_log;

std::ofstream csv_file;

bool debug = false;
bool output = true;

std::string * logarray_filtered_bytime;
int count_lines_in_logarray = 0;

std::string * mailarray_filtered_byinitid;
int id_count_mails_in_nonconcernated_mailarray = 0;

std::string * mailarray_concernated_filtered_byid;
int id_count_mails_in_concernated_mailarray = 0;


std::string filenamestring;

std::string input_filter_options;

//char * upperchar;
//char * lowerchar;

/*Constants*/




/*Functions*/

//String to lowercase - FUNCTION
std::string convert_string_to_lowercase(std::string uppercase)
{
	std::string lowercase = ""/* uppercase*/;
	char * upperchar_c;
	upperchar_c = new (std::nothrow) char[uppercase.size() + 2];
	if (upperchar_c == nullptr)
	{
		std::cerr << "[FEHLER] Speicherzuweisung fehlgeschlagen!" << std::endl;
		exit(-1);
	}
	//char upperchar[uppercase.size()+2];
	//for(int m=0; m < uppercase.size(); m++)
	uppercase.copy(upperchar_c, uppercase.size() + 1);
	upperchar_c[uppercase.size()] = '\0';
	char * lowerchar_c;
	lowerchar_c = new (std::nothrow) char[uppercase.size() + 2];
	if (lowerchar_c == nullptr)
	{
		std::cerr << "[FEHLER] Speicherzuweisung fehlgeschlagen!" << std::endl;
		exit(-1);
	}
	//char lowerchar[uppercase.size() + 2];
	for (int i = 0; i < uppercase.size(); i++)
	{
		switch (upperchar_c[i])
		{
		case 'A':
			lowerchar_c[i] = 'a';
			break;
		case 'B':
			lowerchar_c[i] = 'b';
			break;
		case 'C':
			lowerchar_c[i] = 'c';
			break;
		case 'D':
			lowerchar_c[i] = 'd';
			break;
		case 'E':
			lowerchar_c[i] = 'e';
			break;
		case 'F':
			lowerchar_c[i] = 'f';
			break;
		case 'G':
			lowerchar_c[i] = 'g';
			break;
		case 'H':
			lowerchar_c[i] = 'h';
			break;
		case 'I':
			lowerchar_c[i] = 'i';
			break;
		case 'J':
			lowerchar_c[i] = 'j';
			break;
		case 'K':
			lowerchar_c[i] = 'k';
			break;
		case 'L':
			lowerchar_c[i] = 'l';
			break;
		case 'M':
			lowerchar_c[i] = 'm';
			break;
		case 'N':
			lowerchar_c[i] = 'n';
			break;
		case 'O':
			lowerchar_c[i] = 'o';
			break;
		case 'P':
			lowerchar_c[i] = 'p';
			break;
		case 'Q':
			lowerchar_c[i] = 'q';
			break;
		case 'R':
			lowerchar_c[i] = 'r';
			break;
		case 'S':
			lowerchar_c[i] = 's';
			break;
		case 'T':
			lowerchar_c[i] = 't';
			break;
		case 'U':
			lowerchar_c[i] = 'u';
			break;
		case 'V':
			lowerchar_c[i] = 'v';
			break;
		case 'W':
			lowerchar_c[i] = 'w';
			break;
		case 'X':
			lowerchar_c[i] = 'x';
			break;
		case 'Y':
			lowerchar_c[i] = 'y';
			break;
		case 'Z':
			lowerchar_c[i] = 'z';
			break;
		default:
			lowerchar_c[i] = upperchar_c[i];
			break;
		}
		lowercase = "" + lowercase + lowerchar_c[i];
	}
	delete[] lowerchar_c;
	delete[] upperchar_c;
	return lowercase;
}

//logarray-filtered-bytime FUNCTIONS
void write_logarray_filtered_bytime(std::string txt)
{
	logarray_filtered_bytime[count_lines_in_logarray] = txt;
	count_lines_in_logarray++;
	if (debug) { std::cout << "[DEBUG] Count Lines in LogArray = " << count_lines_in_logarray << "" << std::endl; }
}


//mailarray-filtered_byinitid FUNCTIONS
std::string read_mailarray_filtered_byinitid(int i, int j)
{
	return mailarray_filtered_byinitid[i*ARRAY_TABLE_WIDTH_PER_ELEMENT + j];
}
std::string read_mailarray_concernated_filtered_byid(int i, int j)
{
	return mailarray_concernated_filtered_byid[i*ARRAY_TABLE_WIDTH_PER_ELEMENT + j];
}
void write_mailarray_filtered_byinitid(int i, int j, std::string txt)
{
	mailarray_filtered_byinitid[i*ARRAY_TABLE_WIDTH_PER_ELEMENT + j] = txt;
}
void writeall_mailarray_filtered_byinitid(int id, std::string postfix_queue_id_1, std::string postfix_queue_id_2, std::string msg_id, std::string from_mail, std::string to_mail, std::string amavis_scan_id, std::string amavis_mail_id, std::string internal_id, std::string amavis_resp, std::string internal_pipe_resp, std::string relay_exc_resp, std::string time)
{
	std::string id_str = std::to_string(id);
	std::string complete_str = "false";
	write_mailarray_filtered_byinitid(id, 0, id_str); //"0", "1", "2",...
	write_mailarray_filtered_byinitid(id, 1, postfix_queue_id_1); //"LOSHQ7JD79898HIU"
	write_mailarray_filtered_byinitid(id, 2, postfix_queue_id_2); //"LKPKPOP87MKOJ45N"
	write_mailarray_filtered_byinitid(id, 3, msg_id); //"64386487368346.84.23638648"
	write_mailarray_filtered_byinitid(id, 4, from_mail); //"luke.roeper@test.org"
	write_mailarray_filtered_byinitid(id, 5, to_mail); //"luke.roeper@test2.org"
	write_mailarray_filtered_byinitid(id, 6, amavis_scan_id); //"29361.01"
	write_mailarray_filtered_byinitid(id, 7, amavis_mail_id); //"BGDUIZdjhih8789"
	write_mailarray_filtered_byinitid(id, 8, internal_id); //"56787567856788"
	write_mailarray_filtered_byinitid(id, 9, amavis_resp); //"Passed CLEAN"
	write_mailarray_filtered_byinitid(id, 10, internal_pipe_resp); //"status=sent"
	write_mailarray_filtered_byinitid(id, 11, relay_exc_resp); //"status=sent"
	write_mailarray_filtered_byinitid(id, 12, complete_str);//Complete: true/false ("true" / "false")
	write_mailarray_filtered_byinitid(id, 13, time);//Complete: true/false ("true" / "false")
}

//Not to be used seperatly (used by writeall_mailarray_concernated_filtered_byid)
void write_mailarray_concernated_filtered_byid(int i, int j, std::string txt)
{
	/*write into array:
		logarray_filtered_bytime[i][j] --> logarray_filtered_bytime[i*ARRAY_TABLE_WIDTH_PER_ELEMENT+j]
	*/
	mailarray_concernated_filtered_byid[i*ARRAY_TABLE_WIDTH_PER_ELEMENT + j] = txt;
}

void writeall_mailarray_concernated_filtered_byid(int id, std::string postfix_queue_id_1, std::string postfix_queue_id_2, std::string msg_id, std::string from_mail, std::string to_mail, std::string amavis_scan_id, std::string amavis_mail_id, std::string internal_id, std::string amavis_resp, std::string internal_pipe_resp, std::string relay_exc_resp, bool complete_state, std::string time)
{
	std::string id_str = std::to_string(id);
	std::string complete_str = "false";
	if (complete_state == true) { complete_str = "true"; }
	write_mailarray_concernated_filtered_byid(id, 0, id_str); //"0", "1", "2",...
	write_mailarray_concernated_filtered_byid(id, 1, postfix_queue_id_1); //"LOSHQ7JD79898HIU"
	write_mailarray_concernated_filtered_byid(id, 2, postfix_queue_id_2); //"LKPKPOP87MKOJ45N"
	write_mailarray_concernated_filtered_byid(id, 3, msg_id); //"64386487368346.84.23638648"
	write_mailarray_concernated_filtered_byid(id, 4, from_mail); //"luke.roeper@test.org"
	write_mailarray_concernated_filtered_byid(id, 5, to_mail); //"luke.roeper@test2.org"
	write_mailarray_concernated_filtered_byid(id, 6, amavis_scan_id); //"29361.01"
	write_mailarray_concernated_filtered_byid(id, 7, amavis_mail_id); //"BGDUIZdjhih8789"
	write_mailarray_concernated_filtered_byid(id, 8, internal_id); //"56787567856788"
	write_mailarray_concernated_filtered_byid(id, 9, amavis_resp); //"Passed CLEAN"
	write_mailarray_concernated_filtered_byid(id, 10, internal_pipe_resp); //"status=sent"
	write_mailarray_concernated_filtered_byid(id, 11, relay_exc_resp); //"status=sent"
	write_mailarray_concernated_filtered_byid(id, 12, complete_str);//Complete: true/false ("true" / "false")
	write_mailarray_concernated_filtered_byid(id, 13, time);
}

char * log_c;
//ANANLYZE INTERESTING LINES IN LOGARRAY --> mailarray-filtered-byinitid
void analyze_logarray_by_index(int index)
{
	//std::cout << "1";
	std::string log = logarray_filtered_bytime[index];
	//std::cout << " 2";
	std::string time_str = "";
	//std::cout << " 3";

	if (debug) { std::cout << "[DEBUG] log from array=" << log << std::endl; }
	//std::cout << " 4";

	//std::cout << " 5";
	log_c = new (std::nothrow) char[log.size() + 2];
	//std::cout << " 6 --> " << log.size();
	log.copy(log_c, log.size());
	//std::cout << " 7";
	log_c[log.size()] = '\0';
	//std::cout << " 8";

	for (byte k = 7; k <= 14; k++)
	{
		time_str = "" + time_str + log_c[k];
		//std::cout << " 9 --> " << time_str << std::endl;
	}
	if (debug) { std::cout << "[DEBUG] Time = " << time_str << std::endl; }

	//24 Zeichen vom Start der Zeile entfernt
	for (int i = 24; log_c[i] != '\0'; i++)
	{
		if (log_c[i] == 'c' && log_c[i + 1] == 'o' && log_c[i + 2] == 'n' && log_c[i + 3] == 'n' && log_c[i + 4] == 'e' && log_c[i + 5] == 'c' && log_c[i + 6] == 't' && log_c[i + 7] == ' ' && log_c[i + 8] == 'f' && log_c[i + 9] == 'r' && log_c[i + 10] == 'o' && log_c[i + 11] == 'm' && log_c[i + 12] == ' ')
		{
			if (debug) { std::cout << "[DEBUG] UN-Informative Zeile gefunden (Typ= TEXT (dis-)connect-to)" << std::endl; }
			//[...] 'connect from ' [...] //[...] 'disconnect from '
			//Ignorieren
		}
		else if (log_c[i] == ']' && log_c[i + 1] == ':' && log_c[i + 2] == ' ' && log_c[i + 13] == ':' && log_c[i + 14] == ' ' && log_c[i + 15] == 'c' && log_c[i + 16] == 'l' && log_c[i + 17] == 'i' && log_c[i + 18] == 'e' && log_c[i + 19] == 'n' && log_c[i + 20] == 't' && log_c[i + 21] == '=')
		{
			if (debug) { std::cout << "[DEBUG] UN-Informative Zeile gefunden (Typ= postfix-queue-id-1 + TEXT client=)" << std::endl; }
			//[...] ']: XXXXXXXXXX: client=' [...]
			//XXXXXXXXXX = postfix-queue-id-i
			//Ignorieren
		}
		/*else if (log_c[i] == ']' && log_c[i + 1] == ':' && log_c[i + 2] == ' ' && log_c[i + 13] == ':' && log_c[i + 14] == ' ' && log_c[i + 15] == 'm' && log_c[i + 16] == 'e' && log_c[i + 17] == 's' && log_c[i + 18] == 's' && log_c[i + 19] == 'a' && log_c[i + 20] == 'g' && log_c[i + 21] == 'e' && log_c[i + 22] == '-' && log_c[i + 23] == 'i' && log_c[i + 24] == 'd')
		{
			//[...] ']: XXXXXXXXXX: message-id=<XXXXXXXXX.XX.XXXXXXXXXXXXX@localhost>' [...]
			if (debug) { std::cout << "[DEBUG] Informative Zeile gefunden (Typ= postfix-queue-id-1 + message-id)" << std::endl; }
			std::string postfix_queue_id_1, message_id, low_postfix_queue_id_1, low_message_id = "";
			for (int h = i + 3; log_c[h - 1] != ':'; h++)
			{
				if (log_c[h] == ':' || log_c[h] == ' ')
				{
					break;
				}
				else
				{
					postfix_queue_id_1 = "" + postfix_queue_id_1 + log_c[h];
				}
			}
			for (int h = i + 27; log_c[h - 1] != '>'; h++)
			{
				if (log_c[h] == '>' || log_c[h] == ' ')
				{
					break;
				}
				else
				{
					message_id = "" + message_id + log_c[h];
				}
			}
			//convert to lowercase
			low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
			low_message_id = convert_string_to_lowercase(message_id);
			if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_message_id << std::endl; }
			//write to table
			bool is_second_queue_id = false;
			for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
			{
				//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle message-id = message-id der Line': " << read_mailarray_filtered_byinitid(k, 3) << " - " << low_message_id << std::endl;}
				if (read_mailarray_filtered_byinitid(k, 3) == low_message_id)
				{
					//if(debug){std::cout << "[DEBUG] message-id's identisch"<< std::endl;}
					//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle queue-id != queue-id-1 der Line': " << read_mailarray_filtered_byinitid(k, 1) << " - " << low_postfix_queue_id_1 << std::endl;}
					if (read_mailarray_filtered_byinitid(k, 1) != low_postfix_queue_id_1)
					{
						is_second_queue_id = true;
						//if(debug){std::cout << "[DEBUG] queue-id's NICHT identisch"<< std::endl;}
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] queue-id's identisch"<< std::endl;}
					}
				}
				else
				{
					//if(debug){std::cout << "[DEBUG] message-id's NICHT identisch"<< std::endl;}
				}
			}
			if (is_second_queue_id)
			{
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_1, low_message_id, "null", "null", "null", "null", "null", "null", "null", "null", time_str);
			}
			else
			{
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, "null", low_message_id, "null", "null", "null", "null", "null", "null", "null", "null", time_str);
			}
			id_count_mails_in_nonconcernated_mailarray++;

		}*/
		/*else if (log_c[i] == ']' && log_c[i + 1] == ':' && log_c[i + 2] == ' ' && log_c[i + 13] == ':' && log_c[i + 14] == ' ' && log_c[i + 15] == 'w' && log_c[i + 16] == 'a' && log_c[i + 17] == 'r' && log_c[i + 18] == 'n' && log_c[i + 19] == 'i' && log_c[i + 20] == 'n' && log_c[i + 21] == 'g' && log_c[i + 22] == ':' && log_c[i + 23] == ' ' && log_c[i + 24] == 'h')
		{
			//[...] ']: XXXXXXXXXX: warning: header Subject: XYZ from XYZ[0.0.0.0]; from=<XXXXXXXXXX> to=<YYYYYYYYYYYY>' [...]
			if (debug) { std::cout << "[DEBUG] Informative Zeile gefunden (Typ= postfix-queue-id-1 + from-mail + to-mail)" << std::endl; }
			std::string postfix_queue_id_1, from_mail, to_mail, low_postfix_queue_id_1, low_from_mail, low_to_mail = "";
			//std::cout << " 10";
			for (int h = i + 3; log_c[h - 1] != ':'; h++)
			{
				//std::cout << " 11";
				if (log_c[h] == ':' || log_c[h] == ' ')
				{
					//std::cout << " 11a";
					break;
				}
				else
				{
					//std::cout << " 11b";
					postfix_queue_id_1 = "" + postfix_queue_id_1 + log_c[h];
				}
			}
			//std::cout << " 12";
			for (int h = i + 27; log_c[h] != '\0'; h++)
			{
				if (log_c[h] == 'f' && log_c[h + 1] == 'r' && log_c[h + 2] == 'o' && log_c[h + 3] == 'm' && log_c[h + 4] == '=' && log_c[h + 5] == '<')
				{
					for (int k = h + 6; log_c[k] != '>'; k++)
					{
						from_mail = "" + from_mail + log_c[k];
					}
				}
				if (log_c[h] == 't' && log_c[h + 1] == 'o' && log_c[h + 2] == '=' && log_c[h + 3] == '<')
				{
					for (int m = h + 4; log_c[m] != '>'; m++)
					{
						to_mail = "" + to_mail + log_c[m];
					}
				}

			}
			//std::cout << " 13";
			//convert to lowercase
			low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
			//std::cout << " 14";
			low_from_mail = convert_string_to_lowercase(from_mail);
			//std::cout << " 15";
			low_to_mail = convert_string_to_lowercase(to_mail);
			//std::cout << " 16";
			if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_from_mail << " + " << low_to_mail << std::endl; }
			//write to table
			//std::cout << " 17";
			bool is_second_queue_id = false;
			for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
			{
				//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle message-id = message-id der Line': " << read_mailarray_filtered_byinitid(k, 3) << " - " << low_message_id << std::endl;}
				if (read_mailarray_filtered_byinitid(k, 2) == low_postfix_queue_id_1)
				{
					is_second_queue_id = true;
				}
				else
				{
					//if(debug){std::cout << "[DEBUG] message-id's NICHT identisch"<< std::endl;}
				}
			}
			//std::cout << " 18";
			if (is_second_queue_id)
			{
				//std::cout << " 18a1";
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_1, "null", low_from_mail, low_to_mail, "null", "null", "null", "null", "null", "null", time_str);
				//std::cout << " 18a2";
			}
			else
			{
				//std::cout << " 18b1";
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, "null", "null", low_from_mail, low_to_mail, "null", "null", "null", "null", "null", "null", time_str);
				//std::cout << " 18b2";
			}
			//std::cout << " 19";
			id_count_mails_in_nonconcernated_mailarray++;
		}*/
		/*else if (log_c[i] == ']' && log_c[i + 1] == ':' && log_c[i + 2] == ' ' && log_c[i + 3] == '(' && log_c[i + 12] == ')' && log_c[i + 13] == ' ' && log_c[i + 14] == 'C' && log_c[i + 15] == 'h' && log_c[i + 16] == 'e' && log_c[i + 17] == 'c' && log_c[i + 18] == 'k' && log_c[i + 19] == 'i' && log_c[i + 20] == 'n' && log_c[i + 21] == 'g' && log_c[i + 22] == ':')
		{
			//[...] ']: (XXamavis-scan-idX-XX) Checking: YYYYYamavis-mail-idYYYYY [0.0.0.0] <ZZZfromZZZ> -> <VVVVVtoVVVVV>' [...]
			if (debug) { std::cout << "[DEBUG] Informative Zeile gefunden (Typ= amavis-scan-id + amavis-mail-id + from-mail + to-mail)" << std::endl; }
			std::string amavis_scan_id, amavis_mail_id, from_mail, to_mail, low_amavis_scan_id, low_amavis_mail_id, low_from_mail, low_to_mail = "";

			for (int h = i; log_c[h] != '\0'; h++)
			{
				if (log_c[h] == ']' && log_c[h + 1] == ':' && log_c[h + 2] == ' ' && log_c[h + 3] == '(')
				{
					for (int k = h + 4; log_c[k] != ')'; k++)
					{
						amavis_scan_id = "" + amavis_scan_id + log_c[k];
					}
				}
				if (log_c[h] == 'g' && log_c[h + 1] == ':' && log_c[h + 2] == ' ')
				{
					for (int k = h + 3; log_c[k] != ' '; k++)
					{
						amavis_mail_id = "" + amavis_mail_id + log_c[k];
					}
				}
				if (log_c[h] == ']' && log_c[h + 1] == ' ' && log_c[h + 2] == '<')
				{
					for (int k = h + 3; log_c[k] != '>'; k++)
					{
						from_mail = "" + from_mail + log_c[k];
					}
				}
				if (log_c[h] == '-' && log_c[h + 1] == '>' && log_c[h + 2] == ' ' && log_c[h + 3] == '<')
				{
					for (int m = h + 4; log_c[m] != '>'; m++)
					{
						to_mail = "" + to_mail + log_c[m];
					}
				}
			}
			//convert to lowercase
			low_amavis_scan_id = convert_string_to_lowercase(amavis_scan_id);
			low_amavis_mail_id = convert_string_to_lowercase(amavis_mail_id);
			low_from_mail = convert_string_to_lowercase(from_mail);
			low_to_mail = convert_string_to_lowercase(to_mail);
			if (debug) { std::cout << "[DEBUG] Werte: " << low_amavis_scan_id << " + " << low_amavis_mail_id << " + " << low_from_mail << " + " << low_to_mail << std::endl; }
			//write to table
			writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", "null", "null", low_from_mail, low_to_mail, low_amavis_scan_id, low_amavis_mail_id, "null", "null", "null", "null", time_str);
			id_count_mails_in_nonconcernated_mailarray++;
		}*/
		//else if (log_c[i] == ' ' && log_c[i + 1] == '{' && log_c[i + 2] == 'R' && log_c[i + 3] == 'e' && log_c[i + 4] == 'l' && log_c[i + 5] == 'a' && log_c[i + 6] == 'y' && log_c[i + 7] == 'e' && log_c[i + 8] == 'd')
		else if (log_c[i+5] == ' ' && log_c[i + 6] == '{' && log_c[i + 7] == 'R' && log_c[i + 8] == 'e' && log_c[i + 9] == 'l' && log_c[i + 10] == 'a' && log_c[i + 11] == 'y' && log_c[i + 12] == 'e' && log_c[i + 13] == 'd')
		{
			//[...] ']: (XXamavis-scan-idX-XX) Passed CLEAN {RelayedInternal}, LOCAL [0.0.0.0]:00 [0.0.0.0] <ZZZfromZZZ> -> <VVVVVtoVVVVV>, Queue-ID: Xpostfix-queue-id-2X, Message-ID: <YYmessage-idYY>, mail_id: Xamavis-mail-idX, Hits: -, size: 31893, queued_as: Xpostfix-queue-id-2X, Subject: "Download Zusammenfassung: 'Dateien (1) von Pia Beyer: Werther_Premiere.mp4  '", From: Datenaustauschserver_der_Stadt_Augsburg_<austausch@augsburg.de>, 67 ms' [...]
			//[...] ']: (XXamavis-scan-idX-XX) Passed CLEAN {RelayedInbound}, <ZZZfromZZZ> -> <VVVVVtoVVVVV>, Queue-ID: Xpostfix-queue-id-2X, Message-ID: <YYmessage-idYY>, mail_id: Xamavis-mail-idX, Hits: -, size: 31893, queued_as: Xpostfix-queue-id-2X, Subject: "Download Zusammenfassung: 'Dateien (1) von Pia Beyer: Werther_Premiere.mp4  '", From: Datenaustauschserver_der_Stadt_Augsburg_<austausch@augsburg.de>, 67 ms' [...]
			//[...] ']: (XXamavis-scan-idX-XX) Passed CLEAN {RelayedObound}, LOCAL [0.0.0.0]:00 [0.0.0.0] <ZZZfromZZZ> -> <VVVVVtoVVVVV>, Queue-ID: Xpostfix-queue-id-2X, Message-ID: <YYmessage-idYY>, mail_id: Xamavis-mail-idX, Hits: -, size: 31893, queued_as: Xpostfix-queue-id-2X, Subject: "Download Zusammenfassung: 'Dateien (1) von Pia Beyer: Werther_Premiere.mp4  '", From: Datenaustauschserver_der_Stadt_Augsburg_<austausch@augsburg.de>, 67 ms' [...]
			if (debug) { std::cout << "[DEBUG] Informative Zeile gefunden (Typ= amavis-scan-id + amavis-result + from-mail + to-mail + postfix-queue-id-1 + message-id + amavis-mail-id + postfix-queue-id-2)" << std::endl; }
			std::string amavis_scan_id, amavis_result, from_mail, to_mail, postfix_queue_id_1, message_id, amavis_mail_id, postfix_queue_id_2 = "";
			std::string low_amavis_scan_id, low_amavis_result, low_from_mail, low_to_mail, low_postfix_queue_id_1, low_message_id, low_amavis_mail_id, low_postfix_queue_id_2 = "";

			for (int h = i-24/*40-50*/; log_c[h] != '"' && log_c[h] != '\0' /*&& log_c[h+1] != 'u' && log_c[h+2] != 'b' && log_c[h+3] != 'j' && log_c[h+4] != 'e' && log_c[h+5] != 'c' && log_c[h+6] != 't'*/; h++)
			{
				if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 'F' && log_c[h + 3] == 'r' && log_c[h + 4] == 'o' && log_c[h + 5] == 'm' && log_c[h + 6] == ':' && log_c[h + 7] == ' ')
				{
					break;
				}
				//                std::cout << h << std::endl;
				//                std::cout << log_c[h]  <<  log_c[h+1]  <<  log_c[h+2]  <<  log_c[h+3]  <<  log_c[h+4]  <<  log_c[h+5]  <<  log_c[h+6]  <<  log_c[h+7]  <<  log_c[h+8]  <<  log_c[h+9]  <<  log_c[h+10]  <<  log_c[h+11]  <<  log_c[h+12]  <<  log_c[h+13]  <<  log_c[h+14] << std::endl;
				if (log_c[h] == ']' && log_c[h + 1] == ':' && log_c[h + 2] == ' ' && log_c[h + 3] == '(')
				{
					for (int k = h + 4; log_c[k] != ')'; k++)
					{
						amavis_scan_id = "" + amavis_scan_id + log_c[k];
					}
				}
				if (log_c[h] == ')' && log_c[h + 1] == ' ')
				{
					for (int k = h + 2; log_c[k + 1] != '{' /* && log_c[k + 2] != 'R'*/; k++)
					{
						amavis_result = "" + amavis_result + log_c[k];
					}
				}
				if (log_c[h] == ']' && log_c[h + 1] == ' ' && log_c[h + 2] == '<')
				{
					for (int k = h + 3; log_c[k] != '>'; k++)
					{
						from_mail = "" + from_mail + log_c[k];
					}
				}
				else if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == '<')
				{
					for (int k = h + 3; log_c[k] != '>'; k++)
					{
						from_mail = "" + from_mail + log_c[k];
					}
				}
				if (log_c[h] == '-' && log_c[h + 1] == '>' && log_c[h + 2] == ' ' && log_c[h + 3] == '<')
				{
					bool to_ended = false;
					for (int m = h + 4; !to_ended; m++)
					{
						if (log_c[m] == '>' && log_c[m + 1] == ',' && log_c[m + 2] == '<')
						{
							to_mail = "" + to_mail + ',' + ' ';
							m += 2;
						}
						else if (log_c[m] == '>' && log_c[m + 1] == ',')
						{
							to_ended = true;
							//break;
						}
						else if (log_c[m] == ' ') {
							to_ended = true;
							//break;
						}
						else
						{
							to_mail = "" + to_mail + log_c[m];
						}
					}
				}
				if (log_c[h] == 'Q' && log_c[h + 1] == 'u' && log_c[h + 2] == 'e' && log_c[h + 3] == 'u' && log_c[h + 4] == 'e' && log_c[h + 5] == '-' && log_c[h + 6] == 'I' && log_c[h + 7] == 'D' && log_c[h + 8] == ':' && log_c[h + 9] == ' ')
				{
					for (int m = h + 10; log_c[m] != ','; m++)
					{
						postfix_queue_id_1 = "" + postfix_queue_id_1 + log_c[m];
					}
				}
				if (log_c[h] == 'M' && log_c[h + 1] == 'e' && log_c[h + 2] == 's' && log_c[h + 3] == 's' && log_c[h + 4] == 'a' && log_c[h + 5] == 'g' && log_c[h + 6] == 'e' && log_c[h + 7] == '-' && log_c[h + 8] == 'I' && log_c[h + 9] == 'D' && log_c[h + 10] == ':' && log_c[h + 11] == ' ' && log_c[h + 12] == '<')
				{
					for (int m = h + 13; log_c[m] != '>' && log_c[m + 1] != ','; m++)
					{
						message_id = "" + message_id + log_c[m];
					}
				}
				if (log_c[h] == 'm' && log_c[h + 1] == 'a' && log_c[h + 2] == 'i' && log_c[h + 3] == 'l' && log_c[h + 4] == '_' && log_c[h + 5] == 'i' && log_c[h + 6] == 'd' && log_c[h + 7] == ':' && log_c[h + 8] == ' ')
				{
					for (int m = h + 9; log_c[m] != ','; m++)
					{
						amavis_mail_id = "" + amavis_mail_id + log_c[m];
					}
				}
				if (log_c[h] == 'q' && log_c[h + 1] == 'u' && log_c[h + 2] == 'e' && log_c[h + 3] == 'u' && log_c[h + 4] == 'e' && log_c[h + 5] == 'd' && log_c[h + 6] == '_' && log_c[h + 7] == 'a' && log_c[h + 8] == 's' && log_c[h + 9] == ':')
				{
					for (int m = h + 11; log_c[m] != ','; m++)
					{
						postfix_queue_id_2 = "" + postfix_queue_id_2 + log_c[m];
					}
				}
			}
			//convert to lowercase
			low_amavis_scan_id = convert_string_to_lowercase(amavis_scan_id);
			low_amavis_result = convert_string_to_lowercase(amavis_result);
			low_from_mail = convert_string_to_lowercase(from_mail);
			low_to_mail = convert_string_to_lowercase(to_mail);
			low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
			low_message_id = convert_string_to_lowercase(message_id);
			low_amavis_mail_id = convert_string_to_lowercase(amavis_mail_id);
			low_postfix_queue_id_2 = convert_string_to_lowercase(postfix_queue_id_2);

			if (low_postfix_queue_id_1 != "")
			{
				if (debug) { std::cout << "[DEBUG] Werte: " << low_amavis_scan_id << " + " << low_amavis_result << " + " << low_from_mail << " + " << low_to_mail << " + " << low_postfix_queue_id_1 << " + " << low_message_id << " + " << low_amavis_mail_id << " + " << low_postfix_queue_id_2 << std::endl; }
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, low_postfix_queue_id_2, low_message_id, low_from_mail, low_to_mail, low_amavis_scan_id, low_amavis_mail_id, "null", low_amavis_result, "null", "null", time_str);
			}
			else
			{
				if (debug) { std::cout << "[DEBUG] Werte: " << low_amavis_scan_id << " + " << low_amavis_result << " + " << low_from_mail << " + " << low_to_mail << " + null + " << low_message_id << " + " << low_amavis_mail_id << " + " << low_postfix_queue_id_2 << std::endl; }
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_2, low_message_id, low_from_mail, low_to_mail, low_amavis_scan_id, low_amavis_mail_id, "null", low_amavis_result, "null", "null", time_str);
			}
			//write to table
			id_count_mails_in_nonconcernated_mailarray++;
		}
		else if (log_c[i] == ']' && log_c[i + 1] == ':' && log_c[i + 2] == ' ' && log_c[i + 13] == ':' && log_c[i + 14] == ' ' && log_c[i + 15] == 't' && log_c[i + 16] == 'o' && log_c[i + 17] == '=' && log_c[i + 18] == '<')
		{
			//[...] ']: XXXXXXXXXX: to=<> [...], status=sent (' [...]
			if (debug) { std::cout << "[DEBUG] Informative Zeile gefunden (Typ= postfix-queue-id-1 + internal-pipe-response/relay-exc-response + (postfix-queue-id-2))" << std::endl; }
			std::string postfix_queue_id_1, internal_pipe_response, relay_exc_response, postfix_queue_id_2, internal_id, message_id, low_postfix_queue_id_1, low_internal_pipe_response, low_relay_exc_response, low_postfix_queue_id_2, low_internal_id, low_message_id = "";
			std::string relay = "";
			for (int h = i + 3; log_c[h - 1] != ':'; h++)
			{
				if (log_c[h] == ':' || log_c[h] == ' ')
				{
					break;
				}
				else
				{
					postfix_queue_id_1 = "" + postfix_queue_id_1 + log_c[h];
				}
			}
			for (int h = i + 27; log_c[h] != '\0'; h++)
			{
				if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 'r' && log_c[h + 3] == 'e' && log_c[h + 4] == 'l' && log_c[h + 5] == 'a' && log_c[h + 6] == 'y' && log_c[h + 7] == '=')
				{
					for (int k = h + 8; log_c[k] != '.' && log_c[k] != ','; k++)
					{
						relay = "" + relay + log_c[k];
					}
				}
			}
			if (relay == "127") //Internal Mail
			{
				for (int h = i + 27; log_c[h] != '\0'; h++)
				{
					if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 's' && log_c[h + 3] == 't' && log_c[h + 4] == 'a' && log_c[h + 5] == 't' && log_c[h + 6] == 'u' && log_c[h + 7] == 's' && log_c[h + 8] == '=')
					{
						for (int k = h + 9; log_c[k + 1] != '('; k++)
						{
							internal_pipe_response = "" + internal_pipe_response + log_c[k];
						}
					}
					if (log_c[h] == 'q' && log_c[h + 1] == 'u' && log_c[h + 2] == 'e' && log_c[h + 3] == 'u' && log_c[h + 4] == 'e' && log_c[h + 5] == 'd' && log_c[h + 6] == ' ' && log_c[h + 7] == 'a' && log_c[h + 8] == 's')
					{
						for (int m = h + 10; log_c[m] != ')'; m++)
						{
							postfix_queue_id_2 = "" + postfix_queue_id_2 + log_c[m];
						}
					}
				}
				//convert to lowercase
				low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
				low_internal_pipe_response = convert_string_to_lowercase(internal_pipe_response);
				low_postfix_queue_id_2 = convert_string_to_lowercase(postfix_queue_id_2);
				if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_internal_pipe_response << " + " << low_postfix_queue_id_2 << std::endl; std::cout << "[DEBUG] Relay:Internes Relay" << std::endl; }
				//write to table
				writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, low_postfix_queue_id_2, "null", "null", "null", "null", "null", "null", "null", low_internal_pipe_response, "null", time_str);
			}
			else if (relay == "exc-smtp-2013") //Incoming Mail
			{
				for (int h = i + 120; log_c[h] != '\0'; h++)
				{
					if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 's' && log_c[h + 3] == 't' && log_c[h + 4] == 'a' && log_c[h + 5] == 't' && log_c[h + 6] == 'u' && log_c[h + 7] == 's' && log_c[h + 8] == '=')
					{
						for (int k = h + 9; log_c[k + 1] != '('; k++)
						{
							relay_exc_response = "" + relay_exc_response + log_c[k];
						}
					}
					/*if (log_c[h]==' ' && log_c[h+1]=='<')
					{
						for(int k=h+2; log_c[k] != '>'; k++)
						{
							message_id = "" + message_id + log_c[k];
						}
					}*/
					//> [InternalId=
					if (log_c[h] == '>' && log_c[h + 1] == ' ' && log_c[h + 2] == '[' && log_c[h + 3] == 'I' && log_c[h + 4] == 'n' && log_c[h + 5] == 't' && log_c[h + 6] == 'e' && log_c[h + 7] == 'r' && log_c[h + 8] == 'n' && log_c[h + 9] == 'a' && log_c[h + 10] == 'l' && log_c[h + 11] == 'I' && log_c[h + 12] == 'd' && log_c[h + 13] == '=')
					{
						for (int m = h + 14; log_c[m] != ','; m++)
						{
							internal_id = "" + internal_id + log_c[m];
						}
					}
				}
				//convert to lowercase
				low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
				low_internal_id = convert_string_to_lowercase(internal_id);
				low_message_id = convert_string_to_lowercase(message_id);
				low_relay_exc_response = convert_string_to_lowercase(relay_exc_response);

				if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_internal_id << " + null + " << low_relay_exc_response << std::endl; std::cout << "[DEBUG] Relay: Eingehende Mails --> exc-smtp-2013" << std::endl; }
				//write to table

				bool is_second_queue_id = false;
				for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
				{
					//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle message-id = message-id der Line': " << read_mailarray_filtered_byinitid(k, 3) << " - " << low_message_id << std::endl;}
					if (read_mailarray_filtered_byinitid(k, 2) == low_postfix_queue_id_1)
					{
						is_second_queue_id = true;
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] message-id's NICHT identisch"<< std::endl;}
					}
				}
				if (is_second_queue_id)
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_1, "null", "null", "null", "null", "null", low_internal_id, "null", "null", low_relay_exc_response, time_str);
				}
				else
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, "null", "null", "null", "null", "null", "null", low_internal_id, "null", "null", low_relay_exc_response, time_str);
				}


			}
			else if (relay == "mailrelay") //Outgoing Mail
			{

				for (int h = i + 120; log_c[h] != '\0'; h++)
				{
					if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 's' && log_c[h + 3] == 't' && log_c[h + 4] == 'a' && log_c[h + 5] == 't' && log_c[h + 6] == 'u' && log_c[h + 7] == 's' && log_c[h + 8] == '=')
					{
						for (int k = h + 9; log_c[k + 1] != '('; k++)
						{
							relay_exc_response = "" + relay_exc_response + log_c[k];
							//if(debug){std::cout << "[DEBUG] Zeichen: '" << log_c[k] << "', Gesamt: '" << relay_exc_response << "'" << std::endl;}
						}
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] Not Found: " << log_c[h+9] << log_c[h+10] << log_c[h+11] << log_c[h+12] << log_c[h+13] << log_c[h+14] << log_c[h+15] << log_c[h+16] << log_c[h+17] << std::endl;}
					}

				}
				//convert to lowercase
				low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
				low_relay_exc_response = convert_string_to_lowercase(relay_exc_response);
				if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_relay_exc_response << std::endl; std::cout << "[DEBUG] Relay: Ausgenende Mails --> mailrelay" << std::endl; }
				//write to table

				bool is_second_queue_id = false;
				for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
				{
					//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle message-id = message-id der Line': " << read_mailarray_filtered_byinitid(k, 3) << " - " << low_message_id << std::endl;}
					if (read_mailarray_filtered_byinitid(k, 2) == low_postfix_queue_id_1)
					{
						is_second_queue_id = true;
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] message-id's NICHT identisch"<< std::endl;}
					}
				}
				if (is_second_queue_id)
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_1, "null", "null", "null", "null", "null", "null", "null", "null", low_relay_exc_response, time_str);
				}
				else
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, "null", "null", "null", "null", "null", "null", "null", "null", "null", low_relay_exc_response, time_str);
				}



			}
			else if (relay == "local") //MTA-Innerhalb (root --> root)
			{

				for (int h = i + 90; log_c[h] != '\0'; h++)
				{
					if (log_c[h] == ',' && log_c[h + 1] == ' ' && log_c[h + 2] == 's' && log_c[h + 3] == 't' && log_c[h + 4] == 'a' && log_c[h + 5] == 't' && log_c[h + 6] == 'u' && log_c[h + 7] == 's' && log_c[h + 8] == '=')
					{
						for (int k = h + 9; log_c[k + 1] != '('; k++)
						{
							relay_exc_response = "" + relay_exc_response + log_c[k];
							//if(debug){std::cout << "[DEBUG] Zeichen: '" << log_c[k] << "', Gesamt: '" << relay_exc_response << "'" << std::endl;}
						}
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] Not Found: " << log_c[h+9] << log_c[h+10] << log_c[h+11] << log_c[h+12] << log_c[h+13] << log_c[h+14] << log_c[h+15] << log_c[h+16] << log_c[h+17] << std::endl;}
					}

				}
				//convert to lowercase
				low_postfix_queue_id_1 = convert_string_to_lowercase(postfix_queue_id_1);
				low_relay_exc_response = convert_string_to_lowercase(relay_exc_response);
				if (debug) { std::cout << "[DEBUG] Werte: " << low_postfix_queue_id_1 << " + " << low_relay_exc_response << std::endl; std::cout << "[DEBUG] Relay: MTA-Interne-Mails --> local" << std::endl; }
				//write to table

				bool is_second_queue_id = false;
				for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
				{
					//if(debug){std::cout << "[DEBUG] Prüfe Array-Lines auf 'aktuelle message-id = message-id der Line': " << read_mailarray_filtered_byinitid(k, 3) << " - " << low_message_id << std::endl;}
					if (read_mailarray_filtered_byinitid(k, 2) == low_postfix_queue_id_1)
					{
						is_second_queue_id = true;
					}
					else
					{
						//if(debug){std::cout << "[DEBUG] message-id's NICHT identisch"<< std::endl;}
					}
				}
				if (is_second_queue_id)
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, "null", low_postfix_queue_id_1, "null", "null", "null", "null", "null", "null", "null", "null", low_relay_exc_response, time_str);
				}
				else
				{
					writeall_mailarray_filtered_byinitid(id_count_mails_in_nonconcernated_mailarray, low_postfix_queue_id_1, "null", "null", "null", "null", "null", "null", "null", "null", "null", low_relay_exc_response, time_str);
				}



			}
			else
			{
				std::cerr << "[WARNUNG] Unbekanntes Relay: '" << relay << "'" << std::endl;
			}
			//std::cout << "98";
			id_count_mails_in_nonconcernated_mailarray++;
		}
		//std::cout << "98b";
	}
	//std::cout << "99";

	//std::cout << "100" << std::endl;
}

//CONCERNATE MAILS form mailarray-filtered-byinitid --> mailarray-concernated-filtered-byid
void concernate_mailarray_byinitid()
{
	//int done_counter = 0;
	int dotcounter = 0;
	for (int h = 0; h < id_count_mails_in_nonconcernated_mailarray - 1; h++) {
		if (debug) { std::cout << "(0) " << h << std::endl; }
		//if (done_counter >= 10)
		//{
		//	break;
		//}

		std::string postfix_queue_id_1_l8, postfix_queue_id_2_l8, message_id_l8, from_mail_l8, to_mail_l8, amavis_scan_id_l8, amavis_mail_id_l8, internal_id_l8, amavis_response_l8, internal_pipe_response_l8, relay_exc_response_l8 = "";
		std::string is_complete_l8;
		int index_l8;

		std::string postfix_queue_id_1_l9, postfix_queue_id_2_l9, message_id_l9, from_mail_l9, to_mail_l9, amavis_scan_id_l9, amavis_mail_id_l9, internal_id_l9, amavis_response_l9, internal_pipe_response_l9, relay_exc_response_l9 = "";
		std::string is_complete_l9;
		int index_l9;

		std::string postfix_queue_id_1_l10, postfix_queue_id_2_l10, message_id_l10, from_mail_l10, to_mail_l10, amavis_scan_id_l10, amavis_mail_id_l10, internal_id_l10, amavis_response_l10, internal_pipe_response_l10, relay_exc_response_l10 = "";
		std::string is_complete_l10;
		int index_l10;

		if (debug) { std::cout << "(8)" << std::endl; }
		for (int i = h; i < id_count_mails_in_nonconcernated_mailarray; i++)
		{
			//B
			postfix_queue_id_1_l8 = read_mailarray_filtered_byinitid(i, 1);
			postfix_queue_id_2_l8 = read_mailarray_filtered_byinitid(i, 2);
			message_id_l8 = read_mailarray_filtered_byinitid(i, 3);
			from_mail_l8 = read_mailarray_filtered_byinitid(i, 4);
			to_mail_l8 = read_mailarray_filtered_byinitid(i, 5);
			amavis_scan_id_l8 = read_mailarray_filtered_byinitid(i, 6);
			amavis_mail_id_l8 = read_mailarray_filtered_byinitid(i, 7);
			internal_id_l8 = read_mailarray_filtered_byinitid(i, 8);
			amavis_response_l8 = read_mailarray_filtered_byinitid(i, 9);
			internal_pipe_response_l8 = read_mailarray_filtered_byinitid(i, 10);
			relay_exc_response_l8 = read_mailarray_filtered_byinitid(i, 11);
			is_complete_l8 = read_mailarray_filtered_byinitid(i, 12);
			index_l8 = i;
			//B
			if (debug) { std::cout << "(8) Lese " << i; }
			if (is_complete_l8 == "false")
			{
				//done_counter = 0;
				if (postfix_queue_id_1_l8 != "null")
				{
					if (postfix_queue_id_2_l8 != "null")
					{
						if (message_id_l8 != "null")
						{
							//Line 8 #1 (postfix-queue-id-1 existent)
							if (debug) { std::cout << " als Line 8 (+) queue-id '" << postfix_queue_id_1_l8 << " " << postfix_queue_id_2_l8 << "'" << std::endl; }
							break;
						}
						else
						{
							//Line 9
							index_l8 = -1;
							if (debug) { std::cout << " als Line 9 queue-id '" << postfix_queue_id_1_l8 << " " << postfix_queue_id_2_l8 << "'" << std::endl; }
							continue;
						}
					}
					else
					{
						if (message_id_l8 != "null")
						{
							//Line 3
							index_l8 = -1;
							if (debug) { std::cout << " als Line 3 queue-id " << postfix_queue_id_1_l8 << std::endl; }
							continue;
						}
						else
						{
							//Line 4
							index_l8 = -1;
							if (debug) { std::cout << " als Line 4 queue-id " << postfix_queue_id_1_l8 << std::endl; }
							continue;
						}
					}
				}
				else
				{
					if (amavis_scan_id_l8 != "null")
					{
						if (message_id_l8 != "null")
						{
							//Line 8 #2 (postfix-queue-id-1 existent NICHT)
							if (debug) { std::cout << " als Line 8 (+) queue-id " << postfix_queue_id_2_l8 << std::endl; }
							break;
						}
						else
						{
							//Line 5
							index_l8 = -1;
							if (debug) { std::cout << " als Line 5" << std::endl; }
							continue;
						}
					}
					else if (message_id_l8 != "null")
					{
						//Line 6
						index_l8 = -1;
						if (debug) { std::cout << " als Line 6 queue-id " << postfix_queue_id_2_l8 << std::endl; }
						continue;
					}
					else if (relay_exc_response_l8 != "null")
					{
						//Line 10
						index_l8 = -1;
						if (debug) { std::cout << " als Line 10 queue-id " << postfix_queue_id_2_l8 << std::endl; }
						continue;
					}
					else
					{
						if (postfix_queue_id_2_l8 != "null" && to_mail_l8 != "null")
						{
							//Line 7
							index_l8 = -1;
							if (debug) { std::cout << " als Line 7 queue-id " << postfix_queue_id_2_l8 << std::endl; }
							continue;
						}
						else
						{
							//Line unknown
							index_l8 = -1;
							if (debug) { std::cout << " als UNKNOWN LINE queue-id '" << postfix_queue_id_1_l8 << " " << postfix_queue_id_2_l8 << "'" << std::endl; }
							continue;
						}
					}
				}
			}
			else
			{
				index_l8 = -1;
				if (debug) { std::cout << " als is_complete" << std::endl; }
				//done_counter++;
				//std::cout << done_counter;
				continue;
			}
		}
		if (index_l8 == -1) { continue; }

		if (debug) { std::cout << "(9)" << std::endl; }
		for (int i = h; i < id_count_mails_in_nonconcernated_mailarray; i++)
		{
			postfix_queue_id_1_l9 = read_mailarray_filtered_byinitid(i, 1);
			postfix_queue_id_2_l9 = read_mailarray_filtered_byinitid(i, 2);
			message_id_l9 = read_mailarray_filtered_byinitid(i, 3);
			from_mail_l9 = read_mailarray_filtered_byinitid(i, 4);
			to_mail_l9 = read_mailarray_filtered_byinitid(i, 5);
			amavis_scan_id_l9 = read_mailarray_filtered_byinitid(i, 6);
			amavis_mail_id_l9 = read_mailarray_filtered_byinitid(i, 7);
			internal_id_l9 = read_mailarray_filtered_byinitid(i, 8);
			amavis_response_l9 = read_mailarray_filtered_byinitid(i, 9);
			internal_pipe_response_l9 = read_mailarray_filtered_byinitid(i, 10);
			relay_exc_response_l9 = read_mailarray_filtered_byinitid(i, 11);
			is_complete_l9 = read_mailarray_filtered_byinitid(i, 12);
			index_l9 = i;
			if (debug) { std::cout << "(9) Lese " << i; }
			if (is_complete_l9 == "false")
			{
				if (postfix_queue_id_1_l9 != "null")
				{
					if (postfix_queue_id_2_l9 != "null")
					{
						if (message_id_l9 != "null")
						{
							//Line 8 #1 (postfix-queue-id-1 existent)
							index_l9 = -1;
							if (debug) { std::cout << " als Line 8 queue-id '" << postfix_queue_id_1_l9 << " " << postfix_queue_id_2_l9 << "'" << std::endl; }
							continue;
						}
						else
						{
							//Line 9
							if (postfix_queue_id_2_l9 == postfix_queue_id_2_l8)
							{
								if (debug) { std::cout << " als Line 9 (+) queue-id '" << postfix_queue_id_1_l9 << " " << postfix_queue_id_2_l9 << "'" << std::endl; }
								break;
							}
							else
							{
								index_l9 = -1;
								if (debug) { std::cout << " als Line 9 (-) queue-id '" << postfix_queue_id_1_l9 << " " << postfix_queue_id_2_l9 << "'" << std::endl; }
								continue;
							}

						}
					}
					else
					{
						if (message_id_l9 != "null")
						{
							//Line 3
							index_l9 = -1;
							if (debug) { std::cout << " als Line 3 queue-id " << postfix_queue_id_1_l9 << std::endl; }
							continue;
						}
						else
						{
							//Line 4
							index_l9 = -1;
							if (debug) { std::cout << " als Line 4 queue-id " << postfix_queue_id_1_l9 << std::endl; }
							continue;
						}
					}
				}
				else
				{
					if (amavis_scan_id_l9 != "null")
					{
						if (message_id_l9 != "null")
						{
							//Line 8 #2 (postfix-queue-id-1 existent NICHT)
							index_l9 = -1;
							if (debug) { std::cout << " als Line 8 queue-id " << postfix_queue_id_2_l9 << std::endl; }
							continue;
						}
						else
						{
							//Line 5
							index_l9 = -1;
							if (debug) { std::cout << " als Line 5" << std::endl; }
							continue;
						}
					}
					else if (message_id_l9 != "null")
					{
						//Line 6
						index_l9 = -1;
						if (debug) { std::cout << " als Line 6 queue-id " << postfix_queue_id_2_l9 << std::endl; }
						continue;
					}
					else if (relay_exc_response_l9 != "null")
					{
						//Line 10
						index_l9 = -1;
						if (debug) { std::cout << " als Line 10 queue-id " << postfix_queue_id_1_l9 << std::endl; }
						continue;
					}
					else
					{
						if (postfix_queue_id_2_l9 != "null" && to_mail_l9 != "null")
						{
							//Line 7
							index_l9 = -1;
							if (debug) { std::cout << " als Line 7 queue-id " << postfix_queue_id_2_l9 << std::endl; }
							continue;
						}
						else
						{
							//Line unknown
							index_l9 = -1;
							if (debug) { std::cout << " als UNKNOWN LINE queue-id '" << postfix_queue_id_1_l10 << " " << postfix_queue_id_2_l10 << "'" << std::endl; }
							continue;
						}
					}
				}
			}
			else
			{
				index_l9 = -1;
				if (debug) { std::cout << " als is_complete" << std::endl; }
				continue;
			}
		}
		if (index_l9 == -1) { continue; }

		//if (debug) { std::cout << "(10)" << std::endl; }
		for (int i = h; i < id_count_mails_in_nonconcernated_mailarray; i++)
		{
			postfix_queue_id_1_l10 = read_mailarray_filtered_byinitid(i, 1);
			postfix_queue_id_2_l10 = read_mailarray_filtered_byinitid(i, 2);
			message_id_l10 = read_mailarray_filtered_byinitid(i, 3);
			from_mail_l10 = read_mailarray_filtered_byinitid(i, 4);
			to_mail_l10 = read_mailarray_filtered_byinitid(i, 5);
			amavis_scan_id_l10 = read_mailarray_filtered_byinitid(i, 6);
			amavis_mail_id_l10 = read_mailarray_filtered_byinitid(i, 7);
			internal_id_l10 = read_mailarray_filtered_byinitid(i, 8);
			amavis_response_l10 = read_mailarray_filtered_byinitid(i, 9);
			internal_pipe_response_l10 = read_mailarray_filtered_byinitid(i, 10);
			relay_exc_response_l10 = read_mailarray_filtered_byinitid(i, 11);
			is_complete_l10 = read_mailarray_filtered_byinitid(i, 12);
			index_l10 = i;
			if (debug) { std::cout << "(10) Lese " << i; }
			if (is_complete_l10 == "false")
			{
				if (postfix_queue_id_1_l10 != "null")
				{
					if (postfix_queue_id_2_l10 != "null")
					{
						if (message_id_l10 != "null")
						{
							//Line 8 #1 (postfix-queue-id-1 existent)
							index_l10 = -1;
							if (debug) { std::cout << " als Line 8 queue-id '" << postfix_queue_id_1_l10 << " " << postfix_queue_id_2_l10 << "'" << std::endl; }
							continue;
						}
						else
						{
							//Line 9
							index_l10 = -1;
							if (debug) { std::cout << " als Line 9 queue-id '" << postfix_queue_id_1_l10 << " " << postfix_queue_id_2_l10 << "'" << std::endl; }
							continue;
						}
					}
					else
					{
						if (message_id_l10 != "null")
						{
							//Line 3
							index_l10 = -1;
							if (debug) { std::cout << " als Line 3 queue-id " << postfix_queue_id_1_l10 << std::endl; }
							continue;
						}
						else
						{
							//Line 4
							index_l10 = -1;
							if (debug) { std::cout << " als Line 4 queue-id " << postfix_queue_id_1_l10 << std::endl; }
							continue;
						}
					}
				}
				else
				{
					if (amavis_scan_id_l10 != "null")
					{
						if (message_id_l10 != "null")
						{
							//Line 8 #2 (postfix-queue-id-1 existent NICHT)
							index_l10 = -1;
							if (debug) { std::cout << " als Line 8 queue-id " << postfix_queue_id_2_l10 << std::endl; }
							continue;
						}
						else
						{
							//Line 5
							index_l10 = -1;
							if (debug) { std::cout << " als Line 5" << std::endl; }
							continue;
						}
					}
					else if (message_id_l10 != "null")
					{
						//Line 6
						index_l10 = -1;
						if (debug) { std::cout << " als Line 6 queue-id " << postfix_queue_id_2_l10 << std::endl; }
						continue;
					}
					else if (relay_exc_response_l10 != "null")
					{
						//Line 10
						if (postfix_queue_id_2_l10 == postfix_queue_id_2_l8 && postfix_queue_id_2_l10 == postfix_queue_id_2_l9)
						{
							if (debug) { std::cout << " als Line 10 (+) queue-id " << postfix_queue_id_2_l10 << std::endl; }
							break;
						}
						else
						{
							index_l10 = -1;
							if (debug) { std::cout << " als Line 10 (-) queue-id " << postfix_queue_id_2_l10 << std::endl; }
							continue;
						}

					}
					else
					{
						if (postfix_queue_id_2_l10 != "null" && to_mail_l10 != "null")
						{
							//Line 7
							index_l10 = -1;
							if (debug) { std::cout << " als Line 7 queue-id " << postfix_queue_id_2_l10 << std::endl; }
							continue;
						}
						else
						{
							//Line unknown
							index_l10 = -1;
							if (debug) { std::cout << " als UNKNOWN LINE queue-id '" << postfix_queue_id_1_l10 << " " << postfix_queue_id_2_l10 << "'" << std::endl; }
							continue;
						}
					}
				}

			}
			else
			{
				if (debug) { std::cout << " als is_complete" << std::endl; }
				index_l10 = -1;
				continue;
			}
		}
		if (index_l10 == -1) { continue; }

		if (parameters[1] == "C0" || parameters[1] == "C1" || parameters[1] == "C2" || parameters[1] == "XC0" || parameters[1] == "XC1" || parameters[1] == "XC2")
		{
			h += 2;
		}

		//Zusammenfassen
		std::string time_str = read_mailarray_filtered_byinitid(index_l8, 13);
		write_mailarray_filtered_byinitid(index_l8, 12, "true");
		write_mailarray_filtered_byinitid(index_l9, 12, "true");
		write_mailarray_filtered_byinitid(index_l10, 12, "true");
		std::string internal_response_cache = "" + internal_pipe_response_l9;
		std::string relay_response_cache = "" + relay_exc_response_l10;
		
		for (int k = index_l8; k < id_count_mails_in_nonconcernated_mailarray; k++)
		{
			std::string is_complete = read_mailarray_filtered_byinitid(k, 12);
			if (is_complete == "false")
			{
				std::string postfix_queue_id_1 = read_mailarray_filtered_byinitid(k, 1);
				std::string postfix_queue_id_2 = read_mailarray_filtered_byinitid(k, 2);
				std::string message_id = read_mailarray_filtered_byinitid(k, 3);
				std::string from_mail = read_mailarray_filtered_byinitid(k, 4);
				std::string to_mail = read_mailarray_filtered_byinitid(k, 5);
				std::string amavis_scan_id = read_mailarray_filtered_byinitid(k, 6);
				std::string amavis_mail_id = read_mailarray_filtered_byinitid(k, 7);
				std::string internal_id = read_mailarray_filtered_byinitid(k, 8);
				std::string amavis_response = read_mailarray_filtered_byinitid(k, 9);
				std::string internal_pipe_response = read_mailarray_filtered_byinitid(k, 10);
				std::string relay_exc_response = read_mailarray_filtered_byinitid(k, 11);

				if (internal_pipe_response != "null")
				{
					//Line 9
					if (postfix_queue_id_1 == postfix_queue_id_1_l9 && postfix_queue_id_2 == postfix_queue_id_2_l9)
					{
						if (postfix_queue_id_1 != "" && postfix_queue_id_2 != "")
						{
							internal_response_cache = internal_response_cache + ", " + internal_pipe_response;
							write_mailarray_filtered_byinitid(k, 12, "true");
						}
					}
				}
				if (relay_exc_response != "null")
				{
					//Line 10
					if (postfix_queue_id_2 == postfix_queue_id_2_l10)
					{
						if (postfix_queue_id_2 != "")
						{
							relay_response_cache = relay_response_cache + ", " + relay_exc_response;
							write_mailarray_filtered_byinitid(k, 12, "true");
						}
					}
				}
			}
		}
		writeall_mailarray_concernated_filtered_byid(id_count_mails_in_concernated_mailarray, postfix_queue_id_1_l9, postfix_queue_id_2_l9, message_id_l8, from_mail_l8, to_mail_l8, amavis_scan_id_l8, amavis_mail_id_l8, internal_id_l10, amavis_response_l8, internal_response_cache, relay_response_cache, true, time_str);
		id_count_mails_in_concernated_mailarray++;
		/*for (int k = 0; k < id_count_mails_in_nonconcernated_mailarray; k++)
		{
			if (read_mailarray_filtered_byinitid(k, 1) == postfix_queue_id_1_l9 || read_mailarray_filtered_byinitid(k, 2) == postfix_queue_id_2_l9// || read_mailarray_filtered_byinitid(k, 3) == message_id_l8 || read_mailarray_filtered_byinitid(k, 6) == amavis_scan_id_l8)
			{
				write_mailarray_filtered_byinitid(k, 12, "true");
			}
		}*/
		if (output) {
			dotcounter++;
			std::cout << ":";
			if (dotcounter >= 50 && !debug) { dotcounter = 0; std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"; }
		}
	}
}


//Reads from LogMaster-MTA.defaults and return value of <identifier>
void read_defaults()
{
	std::ifstream defaults_file("LogMaster-MTA.defaults");
	if (defaults_file.is_open())
	{
		char extracted_line_chars[DEFAULTS_LINE_LENGTH];
		std::string extracted_line;
		std::string identifier_line;
		std::string value_line;

		std::cout << "[LogMaster-MTA.defaults] Erwarte Bezeichner '" << default_parameters_identifiers[0] << "', '" << default_parameters_identifiers[1] << "', '" << default_parameters_identifiers[2] /*<<  "', '" << default_parameters_identifiers[3] */ << "'" << std::endl; /*DEBUG*/

		//repeats for every line in file
		while (std::getline(defaults_file, extracted_line))
		{
			identifier_line = "";
			value_line = "";
			int i = 0;
			if (debug) { std::cout << "Lese Zeilen-String: " << extracted_line << std::endl; } /*DEBUG*/
			//copies string into char array
			extracted_line.copy(extracted_line_chars, extracted_line.size() + 1);
			extracted_line_chars[extracted_line.size()] = '\0';

			//grabs chars out of char array (stops before = --> String = "TXTXTX ") and puts them into indentifier string.
			for (i; extracted_line_chars[i] != '='; i++)
			{
				if (debug) { std::cout << "[DEBUG] Lese Einzelzeichen-Char: " << extracted_line_chars[i] << std::endl; } /*DEBUG*/
				if (extracted_line_chars[i] != ' ') {
					identifier_line = "" + identifier_line + extracted_line_chars[i];
				}
			}
			if (debug) { std::cout << "[DEBUG] Lese Bezeichner-String: '" << identifier_line << "'" << std::endl; } /*DEBUG*/


			//checks which identifier it is
			if (identifier_line == default_parameters_identifiers[0] || identifier_line == default_parameters_identifiers[1] || identifier_line == default_parameters_identifiers[2]/* || identifier_line==default_parameters_identifiers[3]*/)
			{
				//grabs chars out of char array (starts after i+2, stops before '\0' --> String != "= TXTXTX --> String = "TXTXTX") and puts them into value string.
				for (i += 2; extracted_line_chars[i] != '\0'; i++)
				{
					if (debug) { std::cout << "[DEBUG] Lese Einzelzeichen-Char: " << extracted_line_chars[i] << std::endl; } /*DEBUG*/
					if (extracted_line_chars[i] == ' ' && extracted_line_chars[i + 1] == ' ')
					{
						break;
					}
					else if (extracted_line_chars[i] == '\0')
					{
						break;
					}
					else
					{
						value_line = "" + value_line + extracted_line_chars[i];
					}
				}
				if (debug) { std::cout << "[DEBUG] Lese Wert-String: '" << identifier_line << "'" << std::endl; } /*DEBUG*/

				if (identifier_line == default_parameters_identifiers[0]) //system
				{
					if (debug) { std::cout << "[DEBUG] LogMaster-MTA.defaults: Bezeichner gefunden: 'system'" << std::endl; } /*DEBUG*/
					std::cout << "[LogMaster-MTA.defaults] Bezeichner 'system' hat den Wert '" << value_line << "' (";
					parameters[0] = value_line;
					if (value_line == "0")
					{
						std::cout << "Windows)" << std::endl;
					}
					else if (value_line == "1")
					{
						std::cout << "Linux)" << std::endl;
					}
					else
					{
						std::cout << "unbekannt)" << std::endl;
					}
				}
				else if (identifier_line == default_parameters_identifiers[1]) //exec-mode
				{
					if (debug) { std::cout << "[DEBUG] LogMaster-MTA.defaults: Bezeichner gefunden: 'exec-mode'" << std::endl; } /*DEBUG*/
					std::cout << "[LogMaster-MTA.defaults] Bezeichner 'exec-mode' hat den Wert '" << value_line << "' (";
					parameters[1] = value_line;
					if (value_line == "0")
					{
						std::cout << "Kommandozeile)" << std::endl;
					}
					else if (value_line == "1")
					{
						std::cout << "Ausgabe in eine Datei)" << std::endl;
					}
					else if (value_line == "2")
					{
						std::cout << "Debug)" << std::endl;
					}
					else if (value_line == "3")
					{
						std::cout << "LogMaster-MTA.defaults reparieren)" << std::endl;
					}
					else if (value_line == "4")
					{
						std::cout << "GUI)" << std::endl;
					}
					else
					{
						std::cout << "unbekannt)" << std::endl;
					}
				}
				else if (identifier_line == default_parameters_identifiers[2]) //source-files-path
				{
					if (debug) { std::cout << "[DEBUG] LogMaster-MTA.defaults: Bezeichner gefunden: 'source-files-path'" << std::endl; } /*DEBUG*/
					std::cout << "[LogMaster-MTA.defaults] Bezeichner 'source-files-path' hat den Wert '" << value_line << "' (0x0)" << std::endl;
					parameters[2] = value_line;
				}
				//                else if (identifier_line==default_parameters_identifiers[3]) //log-file-path
				//                {
				//                    /*std::cout << "Default Parameters File: Found identifier 'log-file-path'" << std::endl;*/ /*DEBUG*/
				//                    std::cout << "Default Parameters File: identifier 'log-file-path' has value '" << value_line << "' (";
				//                    if (value_line != "no-log")
				//                    {
				//                        std::cout << "active)" << std::endl;
				//                    }
				//                    else
				//                    {
				//                        std::cout << "deactivated)" << std::endl;
				//                    }
				//                }
				else
				{
					std::cout << "[Logik-FEHLER] Etwas ist schiefgelaufen... Err-Code=#020" << std::endl;
					if (parameters[0] == "0") { system("pause"); }
					exit(-1);
				}
			}

			else
			{
				std::cerr << "[FEHLER] Die LogMaster-MTA.default Datei scheint nicht richtig formatiert (oder leer) zu sein! Err-Code=#021" << std::endl;
				if (parameters[0] == "0") { system("pause"); }
				exit(-1);
			}

		}

		defaults_file.close();

	}
	else
	{
		std::cerr << "[FEHLER] LogMaster-MTA.defaults kann nicht gefunden oder geoeffnet werden. Bitte etwaige Programme schließen.  Err-Code=#022" << std::endl;
		std::cerr << "Wenn die Datei fehlt, bitte den ExecMode 'repair-defaults' --> 'LogMaster-MTA.exe -System=0 -ExecMode=3' nutzen um die Datei neu zu erstellen." << std::endl;
		if (parameters[0] == "0") { system("pause"); }
		exit(-1);
	}
}

void process_parameters(int argc, char* argv[])
{
	//parameters[x]
	// x = 0 --> System
	// x = 1 --> ExecMode
	// x = 2 --> SourceFiles
	// x = 3 --> (datetimefiltergiven-bool)
	// x = 4 --> Date
	// x = 5 --> Time
	// x = 6 --> Filtertype (0=VON,1=AN,2=ID)
	// x = 7 --> Filtervalue
	// x = 8 --> StartIndex

	if (argc > 1)
	{
		if (debug) { std::cout << "[DEBUG] Angegebene Parameter:" << std::endl; } /*DEBUG*/
		int counter;
		for (counter = 1; counter < argc; counter++)
		{
			if (debug) { std::cout << "[DEBUG] " << argv[counter] << std::endl; } /*DEBUG*/


			bool stage_value = false;
			std::string textinstring = argv[counter];
			char textinchar[200];
			textinstring.copy(textinchar, textinstring.size() + 1);
			textinchar[textinstring.size()] = '\0';
			std::string identifier;
			std::string value;

			//grabs chars out of char array (stops before ' ')
			for (int i = 1; i <= textinstring.size() - 1; i++)
			{
				//std::cout << "[DEBUG] Einzelzeichen " << textinchar[i] << std::endl; /*DEBUG*/
				if (!stage_value && textinchar[i] != '-' && textinchar[i] != '=' && textinchar[i] != ' ') {
					identifier = "" + identifier + textinchar[i];
				}
				else if (!stage_value && textinchar[i] == '=')
				{
					stage_value = true;
				}
				else if (stage_value)
				{
					value = "" + value + textinchar[i];
				}
			}
			//std::cout << "[DEBUG] Bezeichner '" << identifier << "' - Wert '" << value << "'" << std::endl; /*DEBUG*/
			if (identifier == "System" || identifier == "system" || identifier == "sys")
			{
				parameters[0] = value;
				parameters[3] = "0";
			}
			else if (identifier == "ExecMode" || identifier == "execmode" || identifier == "ExceMode" || identifier == "excemode" || identifier == "Mode" || identifier == "mode")
			{
				parameters[1] = value;
				parameters[3] = "0";
			}
			else if (identifier == "SourceFiles" || identifier == "SourceFile" || identifier == "Source" || identifier == "sourcefiles" || identifier == "sourcefile" || identifier == "source")
			{
				parameters[2] = value;
				parameters[3] = "0";
			}
			else if (identifier == "Date" || identifier == "date" || identifier == "Datum" || identifier == "datum")
			{
				parameters[4] = value;
				parameters[3] = "1";
			}
			else if (identifier == "Time" || identifier == "time" || identifier == "Zeit" || identifier == "zeit")
			{
				parameters[5] = value;
				parameters[3] = "1";
			}
			else if (identifier == "VonFilter" || identifier == "vonfilter" || identifier == "vonFilter" || identifier == "Vonfilter")
			{
				parameters[7] = value;
				parameters[6] = "0";
				parameters[3] = "1";
			}
			else if (identifier == "AnFilter" || identifier == "anfilter" || identifier == "anFilter" || identifier == "Anfilter")
			{
				parameters[7] = value;
				parameters[6] = "1";
				parameters[3] = "1";
			}
			else if (identifier == "IdFilter" || identifier == "idfilter" || identifier == "idFilter" || identifier == "Idfilter" || identifier == "IDfilter" || identifier == "IDFilter")
			{
				parameters[7] = value;
				parameters[6] = "2";
				parameters[3] = "1";
			}
			/*else if (identifier == "StartIndex" || identifier == "startIndex" || identifier == "startindex" || identifier == "STARTINDEX")
			{
				parameters[8] = value;
			}*/
			else
			{
				std::cerr << "[FEHLER] Unbekannter Parameter Err-Code=#023" << std::endl;
				if (parameters[0] == "0") { system("pause"); }
				exit(-1);
			}
		}
		if (parameters[6] != "0" && parameters[6] != "1" && parameters[6] != "2")
		{
			parameters[6] = "3";
			parameters[7] = "<enter>";
		}
	}
	else
	{
		read_defaults();
		parameters[3] = "0";
	}
}

void load_log_file(std::string yyyymmdd)
{
	if (debug) { std::cout << "[DEBUG] '" << parameters[2] << "mail-" << yyyymmdd << "'" << std::endl; } /*DEBUG*/
	logfile.open(parameters[2] + "mail-" + yyyymmdd);
}
void close_log_file()
{
	logfile.close();
}

void read_load_whole_log_file()
{
	if (output) { std::cout << std::endl << "Zaehle ["; }
	int dotcounter = 0;
	while (getline(logfile, logfileline))
	{
		if (output) { 
			dotcounter++;
			std::cout << "#";
			if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
		}
		linescounter++;
	}

	if (output) { std::cout << "] (" << linescounter << " Zeilen)" << std::endl; }

	close_log_file();
	load_log_file(filenamestring);
	int linescounter_copy = 0;

	if (logfile.is_open())
	{
		if (linescounter != 0)
		{
			logarray_filtered_bytime = new (std::nothrow) std::string[linescounter];
		}
		else
		{
			std::cerr << std::endl << "[WARNUNG] Keine zeitlich passenden Zeilen gefunden." << std::endl;
			if (parameters[0] == "0") { system("pause"); }
			exit(-1);
		}

		if (logarray_filtered_bytime == nullptr)
		{
			std::cerr << std::endl << "[FEHLER] Dynamischer Speicher konnte nicht zugewiesen werden! Err-Code=#024" << std::endl;
			exit(-1);
		}

		if (output) { std::cout << std::endl << "Lade ["; }

		dotcounter = 0;
		while (getline(logfile, logfileline))
		{
			if (output) {
				dotcounter++;
				std::cout << "#";
				if (dotcounter >= 10) { dotcounter = 0; std::cout << "\b\b\b\b\b\b\b\b\b\b"; }
			}
			write_logarray_filtered_bytime(logfileline);
		}

		if (output) { std::cout << "]" << std::endl; }
	}
}

void read_search_log_file(std::string searchword)
{
	if (output) { std::cout << std::endl << "Zaehle ["; }
	char searchwordChars[7];

	if (searchword.size() == 3) //12:
	{
		searchword.copy(searchwordChars, 3);
		searchwordChars[3] = '\0';
		std::string searchword_short = "";
		for (int k = 0; searchwordChars[k] != '\0'; k++)
		{
			searchword_short = "" + searchword_short + searchwordChars[k];
		}
		int dotcounter = 0;
		while (getline(logfile, logfileline))
		{
			std::string logfileline_short = "";
			char logfileLineChars[21];
			logfileline.copy(logfileLineChars, 19);
			logfileLineChars[20] = '\0';

			for (int h = 7; h < 10; h++)
			{
				logfileline_short = logfileline_short + logfileLineChars[h];
			}

			if (logfileline_short == searchword_short)
			{
				if (debug)
				{
					std::cout << "logfileline = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << "'" << std::endl;
					std::cout << "searchwordline = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << "'" << std::endl;
				}
				else
				{
					if (output) {
						dotcounter++;
						std::cout << "#";
						if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
					}
				}
				linescounter++;
			}
			else
			{
				if (linescounter > 0)
				{
					break;
				}
			}
		}
	}
	else if (searchword.size() == 4) //12:0
	{

		searchword.copy(searchwordChars, 4);
		searchwordChars[4] = '\0';
		std::string searchword_short = "";
		for (int k = 0; searchwordChars[k] != '\0'; k++)
		{
			searchword_short = "" + searchword_short + searchwordChars[k];
		}
		//std::cout << "Suchzeit: " << searchword_short << std::endl;
		int dotcounter = 0;
		while (getline(logfile, logfileline))
		{
			std::string logfileline_short = "";
			char logfileLineChars[21];
			logfileline.copy(logfileLineChars, 19);
			logfileLineChars[20] = '\0';
			for (int p = 7; p < 11; p++)
			{
				logfileline_short = logfileline_short + logfileLineChars[p];
			}
			if (logfileline_short == searchword_short)
			{
				if (debug)
				{
					std::cout << "logfileline = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << logfileLineChars[10] << "'" << std::endl;
					std::cout << "searchwordline = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << searchwordChars[3] << "'" << std::endl;
				}
				else
				{
					if (output) {
						dotcounter++;
						std::cout << "#";
						if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
					}
				}
				linescounter++;
			}
			else
			{
				if (linescounter > 0)
				{
					break;
				}
			}
		}
	}
	else if (searchword.size() == 6) //12:00:
	{

		searchword.copy(searchwordChars, 6);
		searchwordChars[6] = '\0';
		std::string searchword_short = "";
		for (int k = 0; searchwordChars[k] != '\0'; k++)
		{
			searchword_short = "" + searchword_short + searchwordChars[k];
		}
		int dotcounter = 0;
		while (getline(logfile, logfileline))
		{
			std::string logfileline_short = "";
			char logfileLineChars[21];
			logfileline.copy(logfileLineChars, 19);
			logfileLineChars[20] = '\0';
			for (int h = 7; h < 13; h++)
			{
				logfileline_short = logfileline_short + logfileLineChars[h];
			}
			if (logfileline_short == searchword_short)
			{
				if (debug)
				{
					std::cout << "logfileline = '" << logfileline_short << "' = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << logfileLineChars[10] << logfileLineChars[11] << logfileLineChars[12] << "'" << std::endl;
					std::cout << "searchwordline = '" << searchword_short << "' = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << searchwordChars[3] << searchwordChars[4] << searchwordChars[5] << "'" << std::endl;
				}
				else
				{
					if (output) {
						dotcounter++;
						std::cout << "#";
						if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
					}
				}
				linescounter++;
			}
			else
			{
				if (linescounter > 0)
				{
					break;
				}
			}
		}
	}
	else
	{
		std::cerr << "Benutzereingabe inkorrekt formatiert!" << std::endl;
		if (parameters[0] == "0" && output) { system("pause"); }
		exit(-1);
	}
	if (output) { std::cout << "] (" << linescounter << " Zeilen)" << std::endl; }

	//XXX
	//exit(0);
	//XXX

	close_log_file();
	load_log_file(filenamestring);
	int linescounter_copy = 0;

	if (logfile.is_open())
	{
		//logarray_filtered_bytime = new (std::nothrow) std::string [linescounter * ARRAY_TABLE_WIDTH_PER_ELEMENT];
		if (linescounter != 0)
		{
			logarray_filtered_bytime = new (std::nothrow) std::string[linescounter];
		}
		else
		{
			std::cerr << std::endl << "[WARNUNG] Keine zeitlich passenden Zeilen gefunden." << std::endl;
			if (parameters[0] == "0") { system("pause"); }
			exit(-1);
		}

		if (logarray_filtered_bytime == nullptr)
		{
			std::cerr << std::endl << "[FEHLER] Dynamischer Speicher konnte nicht zugewiesen werden! Err-Code=#024" << std::endl;
			exit(-1);
		}
		if (output) { std::cout << std::endl << "Loading ["; }


		if (searchword.size() == 3) //12:
		{
			searchword.copy(searchwordChars, 3);
			searchwordChars[3] = '\0';
			std::string searchword_short = "";
			for (int k = 0; searchwordChars[k] != '\0'; k++)
			{
				searchword_short = "" + searchword_short + searchwordChars[k];
			}
			/*int count = 0;*/
			int dotcounter = 0;
			while (getline(logfile, logfileline))
			{
				/*count++;
				if (count < atoi(parameters[8].c_str()))
				{
					continue;
				}*/
				std::string logfileline_short = "";
				char logfileLineChars[751];
				logfileline.copy(logfileLineChars, 749);
				logfileLineChars[750] = '\0';

				for (int h = 7; h < 10; h++)
				{
					logfileline_short = logfileline_short + logfileLineChars[h];
				}

				if (logfileline_short == searchword_short)
				{
					if (debug)
					{
						std::cout << "[DEBUG] Zeile '" << logfileline << "'" << std::endl;
						std::cout << "logfileline = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << "'" << std::endl;
						std::cout << "searchwordline = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << "'" << std::endl;
					}
					else
					{
						if (output) {
							dotcounter++;
							std::cout << "#";
							if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
						}
					}
					write_logarray_filtered_bytime(logfileline);
					linescounter_copy++;
				}
				else
				{
					if (linescounter_copy == linescounter)
					{
						break;
					}
				}
			}
		}
		else if (searchword.size() == 4) //12:0
		{
			searchword.copy(searchwordChars, 4);
			searchwordChars[4] = '\0';
			std::string searchword_short = "";
			for (int k = 0; searchwordChars[k] != '\0'; k++)
			{
				searchword_short = "" + searchword_short + searchwordChars[k];
			}
			//std::cout << "Suchzeit2: " << searchword_short << std::endl;
			int dotcounter = 0;
			while (getline(logfile, logfileline))
			{
				std::string logfileline_short = "";
				char logfileLineChars[751];
				logfileline.copy(logfileLineChars, 749);
				logfileLineChars[750] = '\0';
				for (int p = 7; p < 11; p++)
				{
					logfileline_short = logfileline_short + logfileLineChars[p];
				}
				if (logfileline_short == searchword_short)
				{
					if (debug)
					{
						std::cout << "[DEBUG] Zeile '" << logfileline << "'" << std::endl;
						std::cout << "[DEBUG] logfileline = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << logfileLineChars[10] << "'" << std::endl;
						std::cout << "[DEBUG] searchwordline = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << searchwordChars[3] << "'" << std::endl; /*DEBUG*/
					}
					else
					{
						if (output) {
							dotcounter++;
							std::cout << "#";
							if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
						}
					}
					write_logarray_filtered_bytime(logfileline);
					linescounter_copy++;
				}
				else
				{
					if (linescounter_copy == linescounter)
					{
						break;
					}
				}
			}
		}
		else if (searchword.size() == 6) //12:00:
		{
			searchword.copy(searchwordChars, 6);
			searchwordChars[6] = '\0';
			std::string searchword_short = "";
			for (int k = 0; searchwordChars[k] != '\0'; k++)
			{
				searchword_short = "" + searchword_short + searchwordChars[k];
			}
			int dotcounter = 0;
			while (getline(logfile, logfileline))
			{
				std::string logfileline_short = "";
				char logfileLineChars[751];
				logfileline.copy(logfileLineChars, 749);
				logfileLineChars[750] = '\0';
				for (int h = 7; h < 13; h++)
				{
					logfileline_short = logfileline_short + logfileLineChars[h];
				}
				if (logfileline_short == searchword_short)
				{
					if (debug)
					{
						std::cout << "[DEBUG] Zeile '" << logfileline << "'" << std::endl;
						std::cout << "logfileline = '" << logfileLineChars[7] << logfileLineChars[8] << logfileLineChars[9] << logfileLineChars[10] << logfileLineChars[11] << logfileLineChars[12] << "'" << std::endl;
						std::cout << "searchwordline = '" << searchwordChars[0] << searchwordChars[1] << searchwordChars[2] << searchwordChars[3] << searchwordChars[4] << searchwordChars[5] << "'" << std::endl;
					}
					else
					{
						if (output) {
							dotcounter++;
							std::cout << "#";
							if (dotcounter >= 10) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
						}
					}
					write_logarray_filtered_bytime(logfileline);
					linescounter_copy++;
				}
				else
				{
					if (linescounter_copy == linescounter)
					{
						break;
					}
				}
			}
		}
		else
		{
			std::cerr << "Benutzereingabe inkorrekt formatiert!" << std::endl;
			if (parameters[0] == "0") { system("pause"); }
			exit(-1);
		}
	}
	if (output) { std::cout << "]" << std::endl; }
}


//open csv-export-file
bool open_csv(std::string filename)
{
	csv_file.open("LogMaster-MTA." + filename + ".csv");
	if (!csv_file.is_open()) {
		return false;
	}
	else
	{
		return true;
	}
}

//write given 1D-Arraydata into csv-file
void write_csv(int index, int filtertype, std::string filtervalue)
{
	std::string datastring = "";

	if (filtertype == -1)
	{
		//std::cout << "Filtertype=-1" << std::endl;
		for (int i = 0; i < ARRAY_TABLE_WIDTH_PER_ELEMENT; i++)
		{
			//std::cout << "[DEBUG] write_csv (" << index << "," << i << "): '" << datastring << "' + '" << read_mailarray_concernated_filtered_byid(index*ARRAY_TABLE_WIDTH_PER_ELEMENT, i) << "'" << std::endl;
			if (i != 0)
			{
				datastring = datastring + ";" + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
			}
			else
			{
				datastring = datastring + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
			}
		}
		//std::cout << "[DEBUG] write_csv (" << index << "): '" << datastring << "'" << std::endl;
		csv_file << datastring << std::endl;
	}
	else if (filtertype == 0) // von-filter
	{
		if (mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + 4] == filtervalue)
		{
			for (int i = 0; i < ARRAY_TABLE_WIDTH_PER_ELEMENT; i++)
			{
				//std::cout << "[DEBUG] write_csv (" << index << "," << i << "): '" << datastring << "' + '" << read_mailarray_concernated_filtered_byid(index*ARRAY_TABLE_WIDTH_PER_ELEMENT, i) << "'" << std::endl;
				if (i != 0)
				{
					datastring = datastring + ";" + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
				else
				{
					datastring = datastring + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
			}
			//std::cout << "[DEBUG] write_csv (" << index << "): '" << datastring << "'" << std::endl;
			csv_file << datastring << std::endl;
		}
	}
	else if (filtertype == 1) // an-filter
	{
		std::string loaded_element = mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + 5];
		//std::cout << "element = " << loaded_element << std::endl;
		char * elementcounter_c;
		elementcounter_c = new (std::nothrow) char[loaded_element.size() + 2];
		loaded_element.copy(elementcounter_c, loaded_element.size() + 2);
		elementcounter_c[loaded_element.size()] = '\0';
		int p = 1;
		for(int x=0; elementcounter_c[x] != '\0'; x++)
		{
			if (elementcounter_c[x] == ',')
			{
				p++;
			}
			if (elementcounter_c[x] == '\0')
			{
				break;
			}
		}
		//std::cout << "p = " << p << std::endl;
		delete[] elementcounter_c;
		std::string * all_single_elements = new (std::nothrow) std::string[p];
		bool reached_end_of_element = false;
		int position_in_element = 0;
		for (int m = 0; !reached_end_of_element && m < p; m++)
		{
			char * element_c;
			element_c = new (std::nothrow) char[loaded_element.size() + 2];
			loaded_element.copy(element_c, loaded_element.size() + 2);
			element_c[loaded_element.size()] = '\0';
			//std::cout << "element_c = ";
			for (position_in_element; true; position_in_element++)
			{
				//std::cout << element_c[position_in_element];
				if (element_c[position_in_element] == '\0') {
					reached_end_of_element = true;
					break;
				}
				else if (element_c[position_in_element] == ',')
				{
					break;
				}
				else
				{
					all_single_elements[m] = "" + all_single_elements[m] + element_c[position_in_element];
				}
			}
			//std::cout << std::endl;
			position_in_element+=2;
			delete[] element_c;
		}
		bool element_success = false;
		for (int n = 0; n < p; n++)
		{
			//std::cout << " '" << all_single_elements[n] << "'";
			if (all_single_elements[n] == filtervalue)
			{
				element_success = true;
				break;
			}
		}
		if (element_success)
		{
			//std::cout << "S";
			for (int i = 0; i < ARRAY_TABLE_WIDTH_PER_ELEMENT; i++)
			{
				//std::cout << "[DEBUG] write_csv (" << index << "," << i << "): '" << datastring << "' + '" << read_mailarray_concernated_filtered_byid(index*ARRAY_TABLE_WIDTH_PER_ELEMENT, i) << "'" << std::endl;
				if (i != 0)
				{
					datastring = datastring + ";" + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
				else
				{
					datastring = datastring + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
			}
			//std::cout << "[DEBUG] write_csv (" << index << "): '" << datastring << "'" << std::endl;
			csv_file << datastring << std::endl;
		}
		delete[] all_single_elements;
		//std::cout << "X" << std::endl;
	}
	else if (filtertype == 2) // id-filter
	{
		if (mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + 3] == filtervalue)
		{
			for (int i = 0; i < ARRAY_TABLE_WIDTH_PER_ELEMENT; i++)
			{
				//std::cout << "[DEBUG] write_csv (" << index << "," << i << "): '" << datastring << "' + '" << read_mailarray_concernated_filtered_byid(index*ARRAY_TABLE_WIDTH_PER_ELEMENT, i) << "'" << std::endl;
				if (i != 0)
				{
					datastring = datastring + ";" + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
				else
				{
					datastring = datastring + mailarray_concernated_filtered_byid[index*ARRAY_TABLE_WIDTH_PER_ELEMENT + i];
				}
			}
			//std::cout << "[DEBUG] write_csv (" << index << "): '" << datastring << "'" << std::endl;
			csv_file << datastring << std::endl;
		}
	}
}

void close_csv()
{
	csv_file.close();
}

void write_time_in_mailarray_concernated()
{
	std::string message_id;
	load_log_file(filenamestring);
	for (int h = 0; h < id_count_mails_in_concernated_mailarray; h++)
	{
		message_id = mailarray_concernated_filtered_byid[h*ARRAY_TABLE_WIDTH_PER_ELEMENT + 3];



	}
	close_log_file();
}


/*main()*/
int main(int argc, char* argv[])
{
	/*logmaster_log.open("LogMaster-MTA.log");
	if (!logmaster_log.is_open()) {
		std::cerr << "[FEHLER] LogMaster-MTA.log konnte nicht geoeffnet werden!" << std::endl;
		exit(-1);
	}*/
	std::cout << "{LogMaster-MTA Version 1.2 by Luke Roeper}" << std::endl;
	//logmaster_log << "{LogMaster-MTA Version 1.1 by Luke Roeper}" << std::endl;
	process_parameters(argc, argv);

	if (parameters[1] == "X0" || parameters[1] == "XC0" || parameters[1] == "0" || parameters[1] == "C0" || parameters[1] == "2" || parameters[1] == "C2" || parameters[1] == "X1" || parameters[1] == "XC1" || parameters[1] == "1" || parameters[1] == "C1")
	{
		if (parameters[1] == "2" || parameters[1] == "C2")
		{
			debug = true;
		}
		if (parameters[1] == "X0" || parameters[1] == "X1" || parameters[1] == "XC0" || parameters[1] == "XC1")
		{
			output = false;
		}

		//Modus Commandline (& Modus Debug)
		if (debug && output)
		{
			if (parameters[1] == "1" || parameters[1] == "C1")
			{
				std::cout << std::endl << "[Modus] Ausgabe in Datei (.csv)" << std::endl << std::endl;
			}
			else
			{
				std::cout << std::endl << "[Modus] Kommandozeile (+Debug)" << std::endl << std::endl;
			}
		}
		else
		{
			if (output) {
				if (parameters[1] == "1" || parameters[1] == "C1")
				{
					std::cout << std::endl << "[Modus] Ausgabe in Datei (.csv)" << std::endl << std::endl;
				}
				else
				{
					std::cout << std::endl << "[Modus] Kommandozeile" << std::endl << std::endl;
				}
			}
		}

		//parameters[x]
		// x = 0 --> System
		// x = 1 --> ExecMode
		// x = 2 --> SourceFiles
		// x = 3 --> (datetimefiltergiven-bool) (0/1)
		// x = 4 --> Date
		// x = 5 --> Time
		// x = 6 --> Filtertype (0=VON,1=AN,2=ID,3=NONE)
		// x = 7 --> Filtervalue (wenn 6 = "3" --> 7 = "<enter>")

		//logmaster_log << "Param: System='" << parameters[0] << "' ExecMode='" << parameters[1] << "' SourceFiles='" << parameters[2] << "' bool_more_args='" << parameters[3] << "' Date='" << parameters[4] << "' Time='" << parameters[5] << "' filter_typ='" << parameters[6] << "' filter_value='" << parameters[7] << "'" << std::endl;

		if (output)
		{
			std::cout << "[Parameter] System = " << parameters[0] << std::endl;
			std::cout << "[Parameter] ExecMode = " << parameters[1] << std::endl;
			std::cout << "[Parameter] SourceFiles = " << parameters[2] << std::endl;
			if (parameters[3] == "1")
			{
				std::cout << "[Parameter] Date = " << parameters[4] << std::endl;
				std::cout << "[Parameter] Time = " << parameters[5] << std::endl;
				if (parameters[6] == "0")
				{
					std::cout << "[Parameter] VonFilter = " << parameters[7] << std::endl;
				}
				else if (parameters[6] == "1")
				{
					std::cout << "[Parameter] AnFilter = " << parameters[7] << std::endl;
				}
				else if (parameters[6] == "2")
				{
					std::cout << "[Parameter] IdFilter = " << parameters[7] << std::endl;
				}
				else
				{
					std::cout << "[Parameter] Filter = none" << std::endl;
				}
			}
			if (parameters[0] == "0") { system("pause"); }
		}

		std::string inputSucheTag_dd_mm_yyyy;
		if (parameters[3] == "1")
		{
			inputSucheTag_dd_mm_yyyy = parameters[4];
		}
		else
		{
			std::cout << std::endl << "[Suchen] Bitte den gewuenschten Tag angeben (Format=DD.MM.JJJJ): ";
			std::cin >> inputSucheTag_dd_mm_yyyy;
		}

		if (debug)
		{
			if (inputSucheTag_dd_mm_yyyy.size() == 10)
			{
				std::cout << "[DEBUG] Input war: '" << inputSucheTag_dd_mm_yyyy << "' (10)" << std::endl; /*DEBUG*/
			}
			else
			{
				std::cout << "[DEBUG] Input war: '" << inputSucheTag_dd_mm_yyyy << "' (" << inputSucheTag_dd_mm_yyyy.size() << ")" << std::endl; /*DEBUG*/
			}
		}

		char inputSucheTag_dd_mm_yyyy_Chars[12];
		inputSucheTag_dd_mm_yyyy.copy(inputSucheTag_dd_mm_yyyy_Chars, inputSucheTag_dd_mm_yyyy.size() + 1);
		inputSucheTag_dd_mm_yyyy_Chars[inputSucheTag_dd_mm_yyyy.size()] = '\0';
		std::string daystring;
		std::string monthstring;
		std::string yearstring;
		bool day_done = false;
		bool month_done = false;

		//grabs chars out of char array (stops before ' ')
		for (int i = 0; inputSucheTag_dd_mm_yyyy_Chars[i] != '\0'; i++)
		{
			if (debug) { std::cout << "[DEBUG] Einzelzeichen " << inputSucheTag_dd_mm_yyyy_Chars[i] << std::endl; } /*DEBUG*/
			if (inputSucheTag_dd_mm_yyyy_Chars[i] == '-' || inputSucheTag_dd_mm_yyyy_Chars[i] == '.' || inputSucheTag_dd_mm_yyyy_Chars[i] == '/' || inputSucheTag_dd_mm_yyyy_Chars[i] == ' ' || inputSucheTag_dd_mm_yyyy_Chars[i] == '_')
			{
				if (day_done == false)
				{
					day_done = true;
				}
				else if (day_done == true && month_done == false)
				{
					month_done = true;
				}
			}
			else if (day_done == false)
			{
				daystring = "" + daystring + inputSucheTag_dd_mm_yyyy_Chars[i];
			}
			else if (day_done == true && month_done == false)
			{
				monthstring = "" + monthstring + inputSucheTag_dd_mm_yyyy_Chars[i];
			}
			else if (day_done == true && month_done == true)
			{
				yearstring = "" + yearstring + inputSucheTag_dd_mm_yyyy_Chars[i];
			}
			else
			{
				break;
			}
			if (debug) { std::cout << "[DEBUG] Bool Status: day_done=" << day_done << " month_done=" << month_done << std::endl; } /*DEBUG*/
		}
		//        int dayint = std::stoi(daystring);
		//        int monthint = std::stoi(monthstring);
		//        int yearint = std::stoi(yearstring);
		//        dayint++;

		std::string cmd_string = "DatumPlusEins.cmd " + daystring + " " + monthstring + " " + yearstring;
		system(cmd_string.c_str());
		std::ifstream updated_date("dateplusone.temp");
		if (updated_date.is_open())
		{
			std::string line;
			getline(updated_date, line);
			//std::cout << line;
			filenamestring = line;
		}
		updated_date.close();
		//filenamestring = "20190304";

		//filenamestring = yearstring + monthstring + daystring;
		if (debug) {
			std::cout << "[DEBUG] Tag = '" << daystring << "'" << std::endl;   /*DEBUG*/
			std::cout << "[DEBUG] Monat = '" << monthstring << "'" << std::endl;
			std::cout << "[DEBUG] Jahr = '" << yearstring << "'" << std::endl;
			std::cout << "[DEBUG] Gesamt = '" << filenamestring << "'" << std::endl;
		}




		load_log_file(filenamestring);
		if (logfile.is_open())
		{
			//HH:MM:SS
			//read_search_log_file("12:"); (12:00:00 - 12:59:59)
			//read_search_log_file("12:1"); (12:10:00 - 12:19:59)
			//read_search_log_file("12:10:"); (12:10:00 - 12:10:59)
			std::string searchstring;
			if (parameters[3] == "1")
			{
				searchstring = parameters[5];
			}
			else
			{
				if (output)
				{
					std::cout << std::endl << "[INFO] Zeitfenster Format: HH:MM:SS (HH=Stunden im 24-Std-Format, MM=Minuten, SS=Sekunden)" << std::endl;
					std::cout << "[INFO] - Gesamt 12:00:00 Uhr bis 12:59:59 = '12:'" << std::endl;
					std::cout << "[INFO] - Gesamt 12:10:00 Uhr bis 12:19:59 = '12:1'" << std::endl;
					std::cout << "[INFO] - Gesamt 12:10:00 Uhr bis 12:10:59 = '12:10:'" << std::endl;
					std::cout << "[INFO] - Gesamter Tag = <ENTER>" << std::endl;
				}
				std::cout << "[Suchen] Bitte das gewuenschte Zeitfenster angeben: ";
				std::cin.ignore(1, '\n');
				getline(std::cin, searchstring, '\n');
			}

			if (searchstring.empty() || searchstring == "day" || searchstring == "DAY")
			{
				read_load_whole_log_file();
			}
			else if (searchstring.size() < 7 && searchstring.size() > 2) {
				if (searchstring.size() == 5)
				{
					std::cout << "[FEHLER] Benutzereingabe (TIME/Zeitfenster) nicht korrekt!" << std::endl;
					if (parameters[0] == "0" && output) { system("pause"); }
					exit(-1);
				}
				read_search_log_file(searchstring);
			}
			else
			{
				std::cout << "[FEHLER] Benutzereingabe (TIME/Zeitfenster) nicht korrekt!" << std::endl;
				if (parameters[0] == "0" && output) { system("pause"); }
				exit(-1);
			}
			close_log_file();
		}
		else
		{
			std::cerr << std::endl << "[FEHLER] Logdatei konnte nicht geoeffnet werden";
			if (parameters[0] == "0" && output) { system("pause"); }
			exit(-1);
		}

		std::string input_filter_options;

		if (parameters[3] == "1")
		{
			if (parameters[6] == "0") //von=
			{
				input_filter_options = "von=" + parameters[7];
			}
			else if (parameters[6] == "1") //an=
			{
				input_filter_options = "an=" + parameters[7];
			}
			else if (parameters[6] == "2") //id=
			{
				input_filter_options = "id=" + parameters[7];
			}
			else if (parameters[6] == "3") //<enter>
			{
				input_filter_options = "<enter>";
			}
		}
		else
		{
			if (output)
			{
				std::cout << std::endl << "[INFO] Filteroptionen: (nur eine!)" << std::endl;
				std::cout << "[INFO] Absender-Filter:    von=max.muster@example.org" << std::endl;
				std::cout << "[INFO] Empfaenger-Filter:  an=max2.mustermann@example.org" << std::endl;
				std::cout << "[INFO] Message-ID-Filter:  id=203cafs374z87r8434g@example.org" << std::endl;
				std::cout << "[INFO] oder <ENTER> fuer eine Auflistung aller E-Mails im Zeitrahmen" << std::endl;
			}
			std::cout << "[Suchen] Wie moechten Sie weiter filtern?: ";
			//std::cin.ignore(60, '\n');
			getline(std::cin, input_filter_options, '\n');
		}

		if (input_filter_options.empty())
		{
			input_filter_options = "<enter>";
			//std::cout << std::endl << "[INFO] Leere Eingabe!" << std::endl;
		}
		else
		{
			input_filter_options = convert_string_to_lowercase(input_filter_options);
			if (debug) { std::cout << "[DEBUG] Input zur kleinschreibung konvertiert: " << input_filter_options; }
		}

		//TODO Analyze logarray_filtered_bytime and write data into mailarray
		if (output) { std::cout << std::endl << "Analysiere ["; }
		/*int mem_normal = linescounter * ARRAY_TABLE_WIDTH_PER_ELEMENT;
		float mem_multiplyer = 2.1;
		int mem_result = (int)(mem_normal * mem_multiplyer);*/
		mailarray_filtered_byinitid = new (std::nothrow) std::string[linescounter * ARRAY_TABLE_WIDTH_PER_ELEMENT];
		if (mailarray_filtered_byinitid == nullptr)
		{
			std::cerr << std::endl << "[FEHLER] Dynamischer Speicher konnte nicht zugewiesen werden! Err-Code=#025" << std::endl;
			exit(-1);
		}
		int dotcounter = 0;
		for (int i = 0; i < linescounter; i++)
		{
			analyze_logarray_by_index(i);
			if (output) {
				dotcounter++;
				std::cout << "#";
				if (dotcounter >= 10 && !debug) { dotcounter = 0; std::cout << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << "" << '\b' << ""; }
			}
		}
		if (output) { std::cout << "] (" << id_count_mails_in_nonconcernated_mailarray << " informative Zeilen gefunden)" << std::endl; }

		if (logarray_filtered_bytime != nullptr)
		{
			delete[] logarray_filtered_bytime;
		}

		/*DEBUG*/ //Output table
		if (debug)
		{
			std::cout << std::endl << "|  Index  |  postfix-queue-id-1  |  postfix-queue-id-2  |  message-id  |  FROM  |  TO  |  Amavis-Scan-ID  |  Amavis-Mail-ID  |  Internal-ID  |  Amavis-Response  |  Internal-Pipe-Response  |  Relay-Exc-Response  |  Entry-Complete?  |  Time  |" << std::endl;
			for (int g = 0; g < id_count_mails_in_nonconcernated_mailarray; g++)
			{
				std::cout << "| " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 0] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 1] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 2] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 3] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 4] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 5] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 6] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 7] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 8] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 9] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 10] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 11] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 12] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 13] << " |" << std::endl;
			}
		}

		//TODO Concernate Data from mailarray(non-concernated) into mailarray(concernated)
		if (output) { std::cout << std::endl << "Fasse Daten Zusammen ["; }
		mailarray_concernated_filtered_byid = new (std::nothrow) std::string[id_count_mails_in_nonconcernated_mailarray * ARRAY_TABLE_WIDTH_PER_ELEMENT];
		if (mailarray_concernated_filtered_byid == nullptr)
		{
			std::cerr << std::endl << "[FEHLER] Dynamischer Speicher konnte nicht zugewiesen werden! Err-Code=#026" << std::endl;
			exit(-1);
		}
		concernate_mailarray_byinitid();
		if (output) { std::cout << "] (" << id_count_mails_in_concernated_mailarray << " E-Mails gefunden)" << std::endl; }

		/*DEBUG*/
		/*std::cout << std::endl << "|  Index  |  postfix-queue-id-1  |  postfix-queue-id-2  |  message-id  |  FROM  |  TO  |  Amavis-Scan-ID  |  Amavis-Mail-ID  |  Internal-ID  |  Amavis-Response  |  Internal-Pipe-Response  |  Relay-Exc-Response  |  Entry-Complete?  |  Time  |" << std::endl;
		for (int g = 0; g < id_count_mails_in_nonconcernated_mailarray; g++)
		{
			std::cout << "| " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 0] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 1] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 2] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 3] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 4] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 5] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 6] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 7] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 8] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 9] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 10] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 11] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 12] << " | " << mailarray_filtered_byinitid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 13] << " |" << std::endl;
		}*/
		/*DEBUG*/

		if (mailarray_filtered_byinitid != nullptr)
		{
			delete[] mailarray_filtered_byinitid;
		}

		//TODO Filter anwenden!
		char filter_options_c[300];
		input_filter_options.copy(filter_options_c, input_filter_options.size() + 1);
		filter_options_c[input_filter_options.size()] = '\0';
		std::string von_filter, an_filter, id_filter = "";
		if (debug) { std::cout << "Filter = '" << input_filter_options << "'" << std::endl; }
		for (int k = 0; k < 2; k++)
		{
			if (debug) { std::cout << "[DEBUG] Lese '" << filter_options_c[k] << filter_options_c[k + 1] << filter_options_c[k + 2] << filter_options_c[k + 3] << "'" << std::endl; }
			if (filter_options_c[k] == 'v' && filter_options_c[k + 1] == 'o' && filter_options_c[k + 2] == 'n' && filter_options_c[k + 3] == '=')
			{
				for (int h = k + 4; filter_options_c[h] != ' ' && filter_options_c[h] != '\0'; h++)
				{
					von_filter = "" + von_filter + filter_options_c[h];
					if (debug) { std::cout << "[DEBUG] von-Filter = '" << von_filter << "'" << std::endl; }
				}
			}
			else if (filter_options_c[k] == 'a' && filter_options_c[k + 1] == 'n' && filter_options_c[k + 2] == '=')
			{
				for (int h = k + 3; filter_options_c[h] != ' ' && filter_options_c[h] != '\0'; h++)
				{
					an_filter = "" + an_filter + filter_options_c[h];
					if (debug) { std::cout << "[DEBUG] an-Filter = '" << an_filter << "'" << std::endl; }
				}
			}
			else if (filter_options_c[k] == 'i' && filter_options_c[k + 1] == 'd' && filter_options_c[k + 2] == '=')
			{
				for (int h = k + 3; filter_options_c[h] != ' ' && filter_options_c[h] != '\0'; h++)
				{
					id_filter = "" + id_filter + filter_options_c[h];
					if (debug) { std::cout << "[DEBUG] id-Filter = '" << id_filter << "'" << std::endl; }
				}
			}
		}

		if (parameters[1] == "1" || parameters[1] == "X1" || parameters[1] == "C1" || parameters[1] == "XC1")
		{
			open_csv(filenamestring);
			csv_file << "CSV-EXPORT aus LogMaster-MTA.exe/out (Filter: "<< von_filter << an_filter << id_filter << ")" << std::endl;
			csv_file << "ID;QUEUE-ID-1;QUEUE-ID-2;MESSAGE-ID;SENDER;EMPFAENGER;AMAVIS-SCAN-ID;AMAVIS-MAIL-ID;INTERNAL-iD;AMAVIS-RESP;INTERNAL-RESP;EXCH/EXTERN-RESP;COMPLETE?;TIME" << std::endl;
			for (int g = 0; g < id_count_mails_in_concernated_mailarray; g++)
			{
				if (von_filter == "" && an_filter == "" && id_filter == "")
				{
					write_csv(g, -1, "");
				}
				else if (von_filter != "")
				{
					write_csv(g, 0, von_filter);
				}
				else if (an_filter != "")
				{
					write_csv(g, 1, an_filter);
				}
				else if (id_filter != "")
				{
					write_csv(g, 2, id_filter);
				}
			}
			close_csv();
		}
		else
		{
			if (von_filter == "" && an_filter == "" && id_filter == "")
			{

				//if (output) { std::cerr << std::endl << std::endl << std::endl << std::endl << "[WARNUNG] Der angegebene Filter konnte nicht angewendet werden. Bitte auf versehentliche Leerzeichen innerhalb ( / vor) der Filterangabe pruefen. Filterangabe = '" << input_filter_options << "'" << std::endl; }
				/*DEBUG*/ //Output table
				//std::cout << std::endl << "|  ID  |  postfix-queue-id-1  |  postfix-queue-id-2  |  message-id  |  FROM  |  TO  |  Amavis-Scan-ID  |  Amavis-Mail-ID  |  Internal-ID  |  Amavis-Response  |  Internal-Pipe-Response  |  Relay-Exc-Response  |  Complete  |  Time  |" << std::endl;
				//for (int g = 0; g < id_count_mails_in_concernated_mailarray; g++)
				//{
				//	std::cout << "[" << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 0] << "] " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 1] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 2] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 3] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 4] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 5] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 6] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 7] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 8] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 9] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 10] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 11] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 12] << " | " << mailarray_concernated_filtered_byid[g*ARRAY_TABLE_WIDTH_PER_ELEMENT + 13] << " |" << std::endl;
				//}

				std::cout /*<< std::endl << std::endl << std::endl*/ << std::endl << "|--------------------------------------------------------------------------------------------------" << std::endl;

				for (int k = 0; k < id_count_mails_in_concernated_mailarray; k++)
				{
					std::cout << "|   ZEITPUNKT: [" << read_mailarray_concernated_filtered_byid(k, 13) << "]" << std::endl;
					std::cout << "|   Absender:   " << read_mailarray_concernated_filtered_byid(k, 4) << std::endl;
					std::cout << "|   Empfaenger: " << read_mailarray_concernated_filtered_byid(k, 5) << std::endl;
					std::cout << "|   Message-ID: " << read_mailarray_concernated_filtered_byid(k, 3) << std::endl;
					std::cout << "|   Queue-ID's: " << read_mailarray_concernated_filtered_byid(k, 1) << " / " << read_mailarray_concernated_filtered_byid(k, 2) << std::endl;
					std::cout << "|   Relay-Intern-Antwort:   " << read_mailarray_concernated_filtered_byid(k, 10) << std::endl;
					std::cout << "|   Virenscan-Antwort:      " << read_mailarray_concernated_filtered_byid(k, 9) << std::endl;
					std::cout << "|   Relay-Exchange-Antwort: " << read_mailarray_concernated_filtered_byid(k, 11) << std::endl;
					std::cout << "|--------------------------------------------------------------------------------------------------" << std::endl;
				}
				if (output) { std::cout << std::endl << std::endl << std::endl; }
			}
			else if (von_filter != "")
			{
				int nonvisible = 0;
				int visible = 0;
				if (output) { std::cerr << std::endl << "[INFO] Wende Filter an: Absender-Filter (" << von_filter << ")" << std::endl << std::endl << std::endl << std::endl; }
				//Liste anzeigen mit angewendetem Filter

				std::cout << std::endl << "|--------------------------------------------------------------------------------------------------" << std::endl;

				for (int k = 0; k < id_count_mails_in_concernated_mailarray; k++)
				{
					if (read_mailarray_concernated_filtered_byid(k, 4) == von_filter)
					{
						std::cout << "|   ZEITPUNKT: [" << read_mailarray_concernated_filtered_byid(k, 13) << "]" << std::endl;
						std::cout << "|   Absender:   " << read_mailarray_concernated_filtered_byid(k, 4) << std::endl;
						std::cout << "|   Empfaenger: " << read_mailarray_concernated_filtered_byid(k, 5) << std::endl;
						std::cout << "|   Message-ID: " << read_mailarray_concernated_filtered_byid(k, 3) << std::endl;
						std::cout << "|   Queue-ID's: " << read_mailarray_concernated_filtered_byid(k, 1) << " / " << read_mailarray_concernated_filtered_byid(k, 2) << std::endl;
						std::cout << "|   Relay-Intern-Antwort:   " << read_mailarray_concernated_filtered_byid(k, 10) << std::endl;
						std::cout << "|   Virenscan-Antwort:      " << read_mailarray_concernated_filtered_byid(k, 9) << std::endl;
						std::cout << "|   Relay-Exchange-Antwort: " << read_mailarray_concernated_filtered_byid(k, 11) << std::endl;
						std::cout << "|--------------------------------------------------------------------------------------------------" << std::endl;
						visible++;
					}
					else
					{
						nonvisible++;
					}
				}
				if (output) { std::cout << std::endl << std::endl << std::endl << std::endl << "[INFO] Filter angewendet: " << visible << " E-Mails sichtbar, " << nonvisible << " E-Mails ausgeblendet" << std::endl; }
			}
			else if (an_filter != "")
			{
				int nonvisible = 0;
				int visible = 0;
				if (output) { std::cerr << std::endl << "[INFO] Wende Filter an: Empfaenger-Filter (" << an_filter << ")" << std::endl; }
				//Liste anzeigen mit angewendetem Filter

				std::cout << std::endl << "|--------------------------------------------------------------------------------------------------" << std::endl;

				for (int k = 0; k < id_count_mails_in_concernated_mailarray; k++)
				{

					std::string loaded_element = read_mailarray_concernated_filtered_byid(k, 5);
					//std::cout << "element = " << loaded_element << std::endl;
					char * elementcounter_c;
					elementcounter_c = new (std::nothrow) char[loaded_element.size() + 2];
					loaded_element.copy(elementcounter_c, loaded_element.size() + 2);
					elementcounter_c[loaded_element.size()] = '\0';
					int p = 1;
					for (int x = 0; elementcounter_c[x] != '\0'; x++)
					{
						if (elementcounter_c[x] == ',')
						{
							p++;
						}
						if (elementcounter_c[x] == '\0')
						{
							break;
						}
					}
					//std::cout << "p = " << p << std::endl;
					delete[] elementcounter_c;
					std::string * all_single_elements = new (std::nothrow) std::string[p];
					bool reached_end_of_element = false;
					int position_in_element = 0;
					for (int m = 0; !reached_end_of_element && m < p; m++)
					{
						char * element_c;
						element_c = new (std::nothrow) char[loaded_element.size() + 2];
						loaded_element.copy(element_c, loaded_element.size() + 2);
						element_c[loaded_element.size()] = '\0';
						//std::cout << "element_c = ";
						for (position_in_element; true; position_in_element++)
						{
							//std::cout << element_c[position_in_element];
							if (element_c[position_in_element] == '\0') {
								reached_end_of_element = true;
								break;
							}
							else if (element_c[position_in_element] == ',')
							{
								break;
							}
							else
							{
								all_single_elements[m] = "" + all_single_elements[m] + element_c[position_in_element];
							}
						}
						//std::cout << std::endl;
						position_in_element += 2;
						delete[] element_c;
					}
					bool element_success = false;
					for (int n = 0; n < p; n++)
					{
						//std::cout << " '" << all_single_elements[n] << "'";
						if (all_single_elements[n] == an_filter)
						{
							element_success = true;
							break;
						}
					}
					if (element_success)
					//if (read_mailarray_concernated_filtered_byid(k, 5) == an_filter)
					{
						std::cout << "|   ZEITPUNKT: [" << read_mailarray_concernated_filtered_byid(k, 13) << "]" << std::endl;
						std::cout << "|   Absender:   " << read_mailarray_concernated_filtered_byid(k, 4) << std::endl;
						std::cout << "|   Empfaenger: " << read_mailarray_concernated_filtered_byid(k, 5) << std::endl;
						std::cout << "|   Message-ID: " << read_mailarray_concernated_filtered_byid(k, 3) << std::endl;
						std::cout << "|   Queue-ID's: " << read_mailarray_concernated_filtered_byid(k, 1) << " / " << read_mailarray_concernated_filtered_byid(k, 2) << std::endl;
						std::cout << "|   Relay-Intern-Antwort:   " << read_mailarray_concernated_filtered_byid(k, 10) << std::endl;
						std::cout << "|   Virenscan-Antwort:      " << read_mailarray_concernated_filtered_byid(k, 9) << std::endl;
						std::cout << "|   Relay-Exchange-Antwort: " << read_mailarray_concernated_filtered_byid(k, 11) << std::endl;
						std::cout << "|--------------------------------------------------------------------------------------------------" << std::endl;
						visible++;
					}
					else
					{
						nonvisible++;
					}
					delete[] all_single_elements;
				}
				if (output) { std::cout << std::endl << "[INFO] Filter angewendet: " << visible << " E-Mails sichtbar, " << nonvisible << " E-Mails ausgeblendet" << std::endl; }
			}
			else if (id_filter != "")
			{
				if (output) { std::cerr << std::endl << "[INFO] Wende Filter an: Message-ID-Filter (" << id_filter << ")" << std::endl; }
				//Liste anzeigen mit angewendetem Filter

				int nonvisible = 0;
				int visible = 0;

				std::cout << std::endl << "|--------------------------------------------------------------------------------------------------" << std::endl;

				for (int k = 0; k < id_count_mails_in_concernated_mailarray; k++)
				{
					if (read_mailarray_concernated_filtered_byid(k, 3) == id_filter)
					{
						std::cout << "|   ZEITPUNKT: [" << read_mailarray_concernated_filtered_byid(k, 13) << "]" << std::endl;
						std::cout << "|   Absender:   " << read_mailarray_concernated_filtered_byid(k, 4) << std::endl;
						std::cout << "|   Empfaenger: " << read_mailarray_concernated_filtered_byid(k, 5) << std::endl;
						std::cout << "|   Message-ID: " << read_mailarray_concernated_filtered_byid(k, 3) << std::endl;
						std::cout << "|   Queue-ID's: " << read_mailarray_concernated_filtered_byid(k, 1) << " / " << read_mailarray_concernated_filtered_byid(k, 2) << std::endl;
						std::cout << "|   Relay-Intern-Antwort:   " << read_mailarray_concernated_filtered_byid(k, 10) << std::endl;
						std::cout << "|   Virenscan-Antwort:      " << read_mailarray_concernated_filtered_byid(k, 9) << std::endl;
						std::cout << "|   Relay-Exchange-Antwort: " << read_mailarray_concernated_filtered_byid(k, 11) << std::endl;
						std::cout << "|--------------------------------------------------------------------------------------------------" << std::endl;
						visible++;
					}
					else
					{
						nonvisible++;
					}
				}
				if (output) { std::cout << std::endl << "[INFO] Filter angewendet: " << visible << " E-Mails sichtbar, " << nonvisible << " E-Mails ausgeblendet" << std::endl; }
			}
		}

		//ENDE
//		if (logarray_filtered_bytime != nullptr)
//		{
//			delete[] logarray_filtered_bytime;
//		}
//		if (mailarray_filtered_byinitid != nullptr)
//		{
//			delete[] mailarray_filtered_byinitid;
//		}
		if (mailarray_concernated_filtered_byid != nullptr)
		{
			delete[] mailarray_concernated_filtered_byid;
		}
		if (output) { std::cout << std::endl << "[LogMaster-MTA] Vorgang Abgeschlossen!" << std::endl; }
	}
	else if (parameters[1] == "3")
	{
		//Modus repair-defaults
		std::cout << std::endl << "[Modus] LogMaster-MTA.defaults reparieren" << std::endl;
		std::cout << "[INFO] Eine moegliche Datei des Namens 'LogMaster-MTA.defaults' wird im Ordner der .exe geloescht!" << std::endl;
		std::cout << "[INFO] Die neue Datei wird im Ordner der LogMaster-MTA.exe generiert!" << std::endl << std::endl;
		if (parameters[0] == "0") { system("pause"); }
		std::ifstream old_defaults_file("LogMaster-MTA.defaults");
		if (old_defaults_file.is_open())
		{
			std::remove("LogMaster-MTA.defaults");
			std::cout << std::endl << "[INFO] Datei 'LogMaster-MTA.defaults' geloescht" << std::endl << std::endl;
		}
		old_defaults_file.close();
		std::string path = "";
		if (parameters[2] != "")
		{
			path = parameters[2];
		}
		else
		{
			std::cout << "Bitte geben Sie den gewuenschten Suchpfad fuer Mail-logdateien an (Format= C:\\Ordner\\Unterordner\\ bzw. /var/log/mail/): " << std::endl;
			std::cin >> path;
		}

		std::ofstream new_defaults_file("LogMaster-MTA.defaults");
		std::cout << "[INFO] Erstelle 'LogMaster-MTA.defaults' [";
		if (parameters[0] == "0")
		{
			new_defaults_file << "system = 0" << std::endl;
			std::cout << ".";
			new_defaults_file << "exec-mode = 0" << std::endl;
			std::cout << ".";
			new_defaults_file << "source-files-path = " << path << std::endl;
			std::cout << ".";
		}
		else
		{
			new_defaults_file << "system = 1" << std::endl;
			std::cout << ".";
			new_defaults_file << "exec-mode = 0" << std::endl;
			std::cout << ".";
			new_defaults_file << "source-files-path = " << path << std::endl;
			std::cout << ".";
		}
		std::cout << ".]" << std::endl;
		new_defaults_file.close();
		if (parameters[0] == "0") { system("pause"); }
		exit(0);
	}
	//	else if (parameters[1] == "4")
	//	{
	//		//Modus GUI
	//		std::cerr << std::endl << "Keine GUI verfuegbar! Bitte verwenden Sie den Kommandozeilenmodus (0)." << std::endl << std::endl;
	//		if (parameters[0] == "0") { system("pause"); }
	//		exit(-1);
	//	}
	else
	{
		std::cerr << std::endl << "Unbekannter Exec-Modus. Wird Abgebrochen!" << std::endl << std::endl;
		if (parameters[0] == "0") { system("pause"); }
		exit(-1);
	}
	delete[] log_c;

	/*logmaster_log.close();*/
	if (parameters[0] == "0") { system("pause"); }
	return 0;
}
