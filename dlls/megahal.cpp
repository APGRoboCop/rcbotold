/*
 *    This file is part of RCBot.
 *
 *    RCBot by Paul Murphy adapted from botman's template 3.
 *
 *    RCBot is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    RCBot is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RCBot; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
 /*

  MegaHAL Stuff

  code from PM's RACC Bot ( http://racc.bots-united.com ) Check it out!
  adapted to rcbot code

  MegaHAL - http://megahal.sourceforge.net

  */

  // RACC - AI development project for first-person shooter games derivated from Valve's Half-Life
  // (http://www.racc-ai.com/)
  //
  // The game to engine interfacing code is based on the work done by Jeffrey 'Botman' Broome
  // (http://planethalflife.com/botman/)
  //
  // This project is partially based on the work done by Eric Bieschke in his BSDbot
  // (http://gamershomepage.com/csbot/)
  //
  // This project is partially based on the work done by Brendan 'Spyro' McCarthy in his ODD Bot
  // (http://oddbot.hlfusion.com/)
  //
  // This project is partially based on the work done by Alistair 'eLiTe' Stewart in his TEAMbot
  // (http://www.planethalflife.com/teambot/)
  //
  // This project is partially based on the work done by Johannes '@$3.1415rin' Lampel in his JoeBot
  // (http://www.joebot.net/)
  //
  // Rational Autonomous Cybernetic Commandos AI
  //
  // bot_chat.cpp
  //

#include "extdll.h"

#ifndef RCBOT_META_BUILD
#include "util.h"
#include "cbase.h"
#else
#include "h_export_meta.h"
#include "meta_api.h"
#endif

#include "bot_const.h"
#include "bot.h"

#include "megahal.h"

extern char* g_argv;
extern CBotGlobals gBotGlobals;

const char* name_in_msg = "%n";

/* anonym001 */
/*#ifndef min
#define min(x,y) (((x) <= (y)) ? (x) : (y))
#endif
#ifndef max
#define max(x,y) (((x) >= (y)) ? (x) : (y))
#endif*/

#ifdef _WIN32
#define strdup _strdup
#endif

void BotChatReply(CBot* pBot, char* szMsg, edict_t* pSender, char* szReplyMsg)
{
	// the purpose of this function is to make the bot keep an eye on what's happening in the
	// chat room, and in case of new messages, think about a possible reply.

	 //callStack.Push("BotChatReply (%x,%x,%x,%x)",pBot,szMsg,pSender,szReplyMsg);
	 // no message!
	if (!szMsg || !*szMsg)
		return;

	// is bot chat allowed AND this message is not from this bot itself ?
	if (gBotGlobals.IsConfigSettingOn(BOT_CONFIG_CHATTING) && pSender != pBot->m_pEdict)
	{
		const size_t iNameLength = std::strlen(pBot->m_szBotName);
		char* szName = new char[sizeof(char) * (iNameLength + 1)];
		RemoveNameTags(pBot->m_szBotName, szName);
		szName[iNameLength] = 0;

		//int i,j;

		strlow(szMsg);
		strlow(szName);

		const char* szNamePos = std::strstr(szMsg, szName);
		const bool bNameInMsg = szNamePos != nullptr;

		const size_t iSenderNameLength = std::strlen(STRING(pSender->v.netname));
		char* szSenderName = new char[sizeof(char) * (iSenderNameLength + 1)];
		RemoveNameTags(STRING(pSender->v.netname), szSenderName);
		szSenderName[iSenderNameLength] = 0;

		/*while ( szNamePos != NULL )
		{
			i = 0;

			std::strstr(szNamePos,szName);
		}*/

		if (bNameInMsg)
		{
			BotFunc_FillString(szMsg, szName, name_in_msg, std::strlen(szMsg));
		}

		// break the new message into an array of words
		HAL_MakeWords(szMsg, pBot->m_Profile.m_HAL->input_words);

		CClient* pClient = gBotGlobals.m_Clients.GetClientByEdict(pSender);
		const int iRep = pBot->m_Profile.m_Rep.GetClientRep(pClient);

		// does the bot feel concerned ? (more chances of replying if its name appears)
		// if real mode is on, then bot chat is affected by bots rep with sender
		// and depends on chat_reply_percent command
		if (gBotGlobals.m_iBotChatReplyPercent && (RANDOM_LONG(0, 100) < gBotGlobals.m_iBotChatReplyPercent && (!
			gBotGlobals.IsConfigSettingOn(BOT_CONFIG_REAL_MODE) ||
			RANDOM_LONG(BOT_MIN_REP, BOT_MAX_REP) < iRep) ||
			/*gBotGlobals.m_Clients*/
			bNameInMsg || UTIL_GetNumClients(false) == 2))
		{
			pBot->m_MegaHALTalkEdict = pSender;

			BotHALGenerateReply(pBot, szReplyMsg); // generate the reply

			BotFunc_FillString(szReplyMsg, name_in_msg, szSenderName, BOT_CHAT_MESSAGE_LENGTH);

			if (std::strcmp(szReplyMsg, szMsg) != 0) // not the exact same message? :-p
				strlow(szReplyMsg); // convert the output string to lowercase
			else
				szReplyMsg[0] = 0;
		}

		// if sender is not a bot then learn from it's message
		// do this after working out a reply so the bots dont just say the same thing back.
		if (!gBotGlobals.IsConfigSettingOn(BOT_CONFIG_CHAT_DONT_LEARN) && UTIL_GetBotPointer(pSender) == nullptr)
			HAL_Learn(pBot->m_Profile.m_HAL->bot_model, pBot->m_Profile.m_HAL->input_words); // only learn from humans

		delete[] szName;
		delete[] szSenderName;

		szName = nullptr;
		szSenderName = nullptr;
	}
}
// from old rcbot
void HumanizeString(char* string)
{
	constexpr int drop_percent = 1;
	constexpr int swap_percent = 1;
	constexpr int capitalise_percent = 1;
	constexpr int lower_percent = 2;

	size_t length = std::strlen(string);
	unsigned int i = 0;
	unsigned int n = 0;
	unsigned int rand;

	while (i < length)
	{
		if (i + 1 < length && RANDOM_LONG(0, 100) < swap_percent)
		{
			const char temp = string[i];

			string[i] = string[i + 1];

			string[i + 1] = temp;

			i += 2;
			continue;
		}

		if ((rand = RANDOM_LONG(0, 100)) < drop_percent ||
			string[n] < '0' && string[n] > '9' && rand < drop_percent * 2)
		{
			n = i;

			while (n < length - 1)
			{
				string[n] = string[n + 1];
				n++;
			}

			string[--length] = 0;

			i++;
			continue;
		}

		if (!isupper(string[i]) && (i == 0 || string[i - 1] == ' '))
		{
			if (RANDOM_LONG(0, 100) < capitalise_percent * 2)
				string[i] = toupper(string[i]);
		}

		if (isupper(string[i]) && RANDOM_LONG(0, 100) < lower_percent)
			string[i] = tolower(string[i]);

		i++;
	}
}

// from old rcbot
void RemoveNameTags(const char* in_string, char* out_string)
{
	unsigned int i = 0; // index of in_string
	int n = 0; // length of out_string

	out_string[0] = 0;

	int tag_size = 0;
	bool space_allowed = false;

	if (in_string == nullptr)
		return;

	const size_t length = std::strlen(in_string);

	if (length > 127)
	{
		if (gBotGlobals.m_iDebugLevels & BOT_DEBUG_THINK_LEVEL)
			ALERT(at_console, "Error : RemoveNameTags(): Input netname is too long!\n");

		return;
	}

	bool inside_tag = false;

	while (i < length)
	{
		char current_char;

		if (inside_tag)
		{
			if (i + 1 < length && in_string[i] == '=' && in_string[i + i] == '-')
			{
				inside_tag = false;
				i += 2;
				continue;
			}
			if (in_string[i] == ')' || in_string[i] == ']' || in_string[i] == '}')
			{
				//char temp = in_string[i];

				inside_tag = false;

				//if ( tag_size == 0 )
				//{
				//	out_string[n++] = tag_start;
				//	out_string[n++] = temp;
				//}

				i++;

				continue;
			}
			tag_size++;
			i++;
			continue;
		}

		if (isalnum(in_string[i]))
		{
			current_char = in_string[i];

			space_allowed = true;
		}
		else if (in_string[i] == '(' || in_string[i] == '[' || in_string[i] == '{')
		{
			inside_tag = true;
			char tag_start = in_string[i];
			tag_size = 0;
			i++;
			continue;
		}
		else if (i + 1 < length && in_string[i] == '-' && in_string[i + i] == '=')
		{
			inside_tag = true;

			i += 2;
			continue;
		}
		else
		{
			if (space_allowed)
			{
				current_char = ' ';
				space_allowed = false;
			}
			else
			{
				i++;
				continue;
			}
		}

		// l33t to normal
		switch (current_char)
		{
		case '5':
			current_char = 's';
			break;
		case '0':
			current_char = 'o';
			break;
		case '7':
			current_char = 't';
			break;
		case '3':
			current_char = 'e';
			break;
		case '1':
			current_char = 'i';
			break;
		case '4':
			current_char = 'a';
			break;
		}

		tag_size = 0;
		out_string[n] = current_char;
		n++;

		i++;
	}

	if (out_string[0] != 0)
	{
		out_string[n] = 0;

		n--;

		while (n > 0 && out_string[n] == ' ')
			out_string[n--] = 0;
	}

	if (out_string[0] == 0)
		std::strcpy(out_string, in_string);

	strlow(out_string);
	/*
		// more 'l33t stuff
		// "l33t" R
		BotFunc_FillString(out_string,"|2","r",length);
		// fat "l33ty" P's I's and M's
		BotFunc_FillString(out_string,"[]D","p",length);
		BotFunc_FillString(out_string,"[]V[]","m",length);
		BotFunc_FillString(out_string,"[]","i",length);

		// http://www.bbc.co.uk/dna/h2g2/A787917
		BotFunc_FillString(out_string,"()","o",length);
		BotFunc_FillString(out_string,"|_|","u",length);
		BotFunc_FillString(out_string,"|)","d",length);
		BotFunc_FillString(out_string,"\\/\\/","w",length);
		BotFunc_FillString(out_string,"$","s",length);
	*/
}

void BotHALGenerateReply(CBot* pBot, char* output)
{
	// this function takes a string of user input and return a string of output which may
	// vaguely be construed as containing a reply to whatever is in the input string.
	// Create an array of keywords from the words in the user's input...

	static char* output_template = nullptr;
	int length = 1;
	int i, j;

	if (output_template == nullptr)
	{
		output_template = static_cast<char*>(std::malloc(sizeof(char)));

		if (output_template == nullptr)
			BotMessage(nullptr, 1, "HAL: BotHALGenerateReply() unable to allocate output\n");
	}

	output_template[0] = 0; // first reset the reply string

	HAL_DICTIONARY* keywords = BotHALMakeKeywords(pBot, pBot->m_Profile.m_HAL->input_words);
	HAL_DICTIONARY* replywords = BotHALBuildReplyDictionary(pBot, keywords);

	const int last_entry = pBot->m_Profile.m_HAL->input_words->size - 1;
	const int last_character = pBot->m_Profile.m_HAL->input_words->entry[last_entry].length - 1;

	// was it a question (i.e. was the last word in the general chat record a question mark ?)
	if (pBot->m_Profile.m_HAL->input_words->entry[last_entry].word[last_character] == '?')
	{
		int tries_count;
		// try ten times to answer something relevant
		for (tries_count = 0; tries_count < 10; tries_count++)
		{
			if (HAL_DictionariesDiffer(pBot->m_Profile.m_HAL->input_words, replywords))
				break; // stop as soon as we've got something to say
			else
				replywords = BotHALBuildReplyDictionary(pBot, keywords); // else think again
		}

		// if we've finally found something to say, generate the reply
		if (tries_count < 10)
		{
			// if no words in the reply dictionary...
			if (replywords->size == 0)
			{
				// the HAL is too young to talk yet
				output[0] = 0;
				return;
			}

			for (i = 0; i < static_cast<int>(replywords->size); ++i)
				length += replywords->entry[i].length;

			char* temp = static_cast<char*>(std::realloc(output_template, sizeof(char) * length));
			if (temp == nullptr)
			{
				BotMessage(nullptr, 1, "HAL: HAL_MakeOutput() unable to reallocate output\n");
				std::free(output_template); // free original block
				// handle error here, possibly with a return or throw [APG]RoboCop[CL]
				return; // or throw an exception
			}

			output_template = temp;

			length = 0;
			for (i = 0; i < static_cast<int>(replywords->size); ++i)
				for (j = 0; j < replywords->entry[i].length; ++j)
					output_template[length++] = replywords->entry[i].word[j];

			if (length > BOT_CHAT_MESSAGE_LENGTH)
				output_template[BOT_CHAT_MESSAGE_LENGTH - 1] = 0; // disallow strings to be longer than BOT_CHAT_MESSAGE_LENGTH chars
			else
				output_template[length] = 0; // terminate the string

			std::strcpy(output, output_template); // then copy the answer
			return;
		}
	}

	// else if we are not paraphrazing, generate a string from the dictionary of reply words
	else if (HAL_DictionariesDiffer(pBot->m_Profile.m_HAL->input_words, replywords))
	{
		// if no words in the reply dictionary...
		if (replywords->size == 0)
		{
			// the HAL is too young to talk yet
			output[0] = 0;
			return;
		}

		for (i = 0; i < static_cast<int>(replywords->size); ++i)
			length += replywords->entry[i].length;

		output_template = static_cast<char*>(std::realloc(output_template, sizeof(char) * length));
		if (output_template == nullptr)
			BotMessage(nullptr, 1, "HAL: HAL_MakeOutput() unable to reallocate output\n");

		length = 0;

		for (i = 0; i < static_cast<int>(replywords->size); ++i)
			for (j = 0; j < replywords->entry[i].length; ++j)
				output_template[length++] = replywords->entry[i].word[j];

		if (length > BOT_CHAT_MESSAGE_LENGTH - 1)
			output_template[BOT_CHAT_MESSAGE_LENGTH - 1] = 0; // disallow strings to be longer than 128 chars
		else
			output_template[length] = 0; // terminate the string

		std::strcpy(output, output_template); // then copy the answer
		return;
	}
}

unsigned short HAL_AddWord(HAL_DICTIONARY* dictionary, HAL_STRING word)
{
	// this function adds a word to a dictionary, and return the identifier assigned to the
	// word. If the word already exists in the dictionary, then return its current identifier
	// without adding it again.

	int i;
	bool found;

	// if the word's already in the dictionary, there is no need to add it
	const int position = HAL_SearchDictionary(dictionary, word, &found);
	// Check if dictionary->index is not nullptr before using it
	if (found && dictionary->index != nullptr)
		return dictionary->index[position];

	// increase the number of words in the dictionary
	dictionary->size++;

	// allocate one more entry for the word index
	if (dictionary->index == nullptr)
		dictionary->index = static_cast<unsigned short*>(std::malloc(sizeof(unsigned short) * dictionary->size));
	else
		dictionary->index = static_cast<unsigned short*>(std::realloc(dictionary->index,
			sizeof(unsigned short) * dictionary->size));

	if (dictionary->index == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddWord() unable to reallocate the dictionary index\n");

	// allocate one more entry for the word array
	if (dictionary->entry == nullptr)
		dictionary->entry = static_cast<HAL_STRING*>(std::malloc(sizeof(HAL_STRING) * dictionary->size));
	else
		dictionary->entry = static_cast<HAL_STRING*>(std::realloc(dictionary->entry,
			sizeof(HAL_STRING) * dictionary->size));

	if (dictionary->entry == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddWord() unable to reallocate the dictionary to %d elements\n", dictionary->size);

	// copy the new word into the word array
	dictionary->entry[dictionary->size - 1].length = word.length;
	dictionary->entry[dictionary->size - 1].word = static_cast<char*>(std::malloc(sizeof(char) * (word.length + 1)));
	if (dictionary->entry[dictionary->size - 1].word == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddWord() unable to allocate the word\n");

	for (i = 0; i < word.length; ++i)
		dictionary->entry[dictionary->size - 1].word[i] = word.word[i];
	dictionary->entry[dictionary->size - 1].word[word.length] = 0;

	// shuffle the word index to keep it sorted alphabetically
	for (i = dictionary->size - 1; i > position; --i)
		dictionary->index[i] = dictionary->index[i - 1];

	// copy the new symbol identifier into the word index
	dictionary->index[position] = static_cast<unsigned short>(dictionary->size) - 1;

	return dictionary->index[position];
}

int HAL_SearchDictionary(HAL_DICTIONARY* dictionary, HAL_STRING word, bool* find)
{
	// search the dictionary for the specified word, returning its position in the index if
	// found, or the position where it should be inserted otherwise

	// if the dictionary is empty, then obviously the word won't be found
	if (dictionary->size == 0)
	{
		*find = false;
		return 0;
	}

	// initialize the lower and upper bounds of the search
	int imin = 0;
	int imax = dictionary->size - 1;

	//bool bDone = false;
	//int iFound = 0;

	// search repeatedly, halving the search space each time, until either the entry is found,
	// or the search space becomes empty
	while (true)
	{
		// see whether the middle element of the search space is greater than, equal to, or
		// less than the element being searched for.
		const int middle = (imin + imax) / 2;
		const int compar = HAL_CompareWords(word, dictionary->entry[dictionary->index[middle]]);

		// if equal then we have found the element. Otherwise halve the search space accordingly
		if (compar == 0)
		{
			*find = true;
			return middle;
		}
		else if (compar > 0)
		{
			if (imax == middle)
			{
				*find = false;
				return middle + 1;
			}

			imin = middle + 1;
		}
		else
		{
			if (imin == middle)
			{
				*find = false;
				return middle;
			}

			imax = middle - 1;
		}
	}
}

unsigned short HAL_FindWord(HAL_DICTIONARY* dictionary, HAL_STRING word)
{
	// this function returns the symbol corresponding to the word specified. We assume that
	// the word with index zero is equal to a NULL word, indicating an error condition.

	bool found;
	const int position = HAL_SearchDictionary(dictionary, word, &found);

	if (found == 1)
		return dictionary->index[position];
	else
		return 0;
}

int HAL_CompareWords(HAL_STRING word1, HAL_STRING word2)
{
	// this function compares two words, and return an integer indicating whether the first
	// word is less than, equal to or greater than the second word

	try
	{
		const int bound = static_cast<int>(std::fmin(word1.length, word2.length));

		for (int i = 0; i < bound; ++i)
			if (toupper(word1.word[i]) != toupper(word2.word[i]))
				return toupper(word1.word[i]) - toupper(word2.word[i]);
	}
	catch (...)
	{
		BotMessage(nullptr, 2, "First chance exception in HAL_CompareWords()\r\n(May be a problem with HAL Files)");
	}
	if (word1.length < word2.length)
		return -1;
	else if (word1.length > word2.length)
		return 1;
	else
		return 0;
}

void HAL_InitializeDictionary(HAL_DICTIONARY* dictionary)
{
	// this function adds dummy words to the dictionary

	constexpr HAL_STRING word = { 7, "<ERROR>" };
	constexpr HAL_STRING end = { 5, "<FIN>" };

	(void)HAL_AddWord(dictionary, word);
	(void)HAL_AddWord(dictionary, end);
}

HAL_DICTIONARY* HAL_NewDictionary()
{
	// this function allocates room for a new dictionary

	HAL_DICTIONARY* dictionary = static_cast<HAL_DICTIONARY*>(std::malloc(sizeof(HAL_DICTIONARY)));
	if (dictionary == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_NewDictionary() unable to allocate dictionary\n");

	dictionary->size = 0;
	dictionary->index = nullptr;
	dictionary->entry = nullptr;

	return dictionary;
}

void HAL_SaveDictionary(std::FILE* file, HAL_DICTIONARY* dictionary)
{
	// this function saves a dictionary to the specified file

	std::fwrite(&dictionary->size, sizeof(unsigned long), 1, file);

	// save each word to the file
	for (int i = 0; i < static_cast<int>(dictionary->size); ++i)
	{
		std::fwrite(&dictionary->entry[i].length, sizeof(unsigned char), 1, file);
		for (int j = 0; j < dictionary->entry[i].length; ++j)
			std::fwrite(&dictionary->entry[i].word[j], sizeof(char), 1, file);
	}
}

void HAL_LoadDictionary(std::FILE* file, HAL_DICTIONARY* dictionary)
{
	// this function loads a dictionary from the specified file

	int size;
	HAL_STRING word;

	if (file == nullptr)
		return;

	std::fread(&size, sizeof(unsigned long), 1, file);

	// load each dictionary word from the file
	for (int i = 0; i < size; ++i)
	{
		std::fread(&word.length, sizeof(unsigned char), 1, file);

		word.word = static_cast<char*>(std::malloc(sizeof(char) * (word.length + 1)));

		if (word.word == nullptr)
			BotMessage(nullptr, 1, "HAL: HAL_LoadDictionary() unable to allocate word\n");

		//for (j = 0; j < word.length; ++j)
		std::fread(word.word, sizeof(char), word.length, file);

		word.word[word.length] = 0;
		//word.word[word.length] = 0;

		HAL_AddWord(dictionary, word);

		//if (word.word != nullptr)
		std::free(word.word);

		word.word = nullptr;
	}
}

HAL_TREE* HAL_NewNode()
{
	// allocate a new node for the n-gram tree, and initialise its contents to sensible values

	// allocate memory for the new node
	HAL_TREE* node = static_cast<HAL_TREE*>(std::malloc(sizeof(HAL_TREE)));
	if (node == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_NewNode() unable to allocate node\n");

	// initialise the contents of the node
	node->symbol = 0;
	node->usage = 0;
	node->count = 0;
	node->branch = 0;
	node->tree = nullptr;

	return node;
}

HAL_MODEL* HAL_NewModel(int order)
{
	// this function creates and initializes a new ngram model

	HAL_MODEL* model = static_cast<HAL_MODEL*>(std::malloc(sizeof(HAL_MODEL)));
	if (model == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_NewModel() unable to allocate model\n");

	model->order = static_cast<unsigned char>(order);
	model->forward = HAL_NewNode();
	model->backward = HAL_NewNode();
	model->context = static_cast<HAL_TREE**>(std::malloc(sizeof(HAL_TREE*) * (order + 2)));
	if (model->context == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_NewModel() unable to allocate context array\n");

	HAL_InitializeContext(model);
	model->dictionary = HAL_NewDictionary();
	HAL_InitializeDictionary(model->dictionary);

	return model;
}

void HAL_UpdateModel(HAL_MODEL* model, int symbol)
{
	// this function uppdates the model with the specified symbol

	// update all of the models in the current context with the specified symbol
	for (int i = model->order + 1; i > 0; --i)
		if (model->context[i - 1] != nullptr)
			model->context[i] = HAL_AddSymbol(model->context[i - 1], static_cast<unsigned short>(symbol));

	return;
}

void HAL_UpdateContext(HAL_MODEL* model, int symbol)
{
	// this function updates the context of the model without adding the symbol

	for (int i = model->order + 1; i > 0; --i)
		if (model->context[i - 1] != nullptr)
			model->context[i] = HAL_FindSymbol(model->context[i - 1], symbol);
}

HAL_TREE* HAL_AddSymbol(HAL_TREE* tree, unsigned short symbol)
{
	// this function updates the statistics of the specified tree with the specified symbol,
	// which may mean growing the tree if the symbol hasn't been seen in this context before

	// search for the symbol in the subtree of the tree node
	HAL_TREE* node = HAL_FindSymbolAdd(tree, symbol);

	// increment the symbol counts
	if (node->count < 65535)
	{
		node->count++;
		tree->usage++;
	}

	return node;
}

HAL_TREE* HAL_FindSymbol(HAL_TREE* node, int symbol)
{
	// this function returns a pointer to the child node, if one exists, which contains symbol

	HAL_TREE* found = nullptr;
	bool found_symbol = false;

	// perform a binary search for the symbol
	const int i = HAL_SearchNode(node, symbol, &found_symbol);
	if (found_symbol)
		found = node->tree[i];

	return found;
}

HAL_TREE* HAL_FindSymbolAdd(HAL_TREE* node, int symbol)
{
	// this function is conceptually similar to HAL_FindSymbol, apart from the fact that if the
	// symbol is not found, a new node is automatically allocated and added to the tree

	HAL_TREE* found;
	bool found_symbol = false;

	// perform a binary search for the symbol. If the symbol isn't found, attach a new sub-node
	// to the tree node so that it remains sorted.
	const int i = HAL_SearchNode(node, symbol, &found_symbol);

	if (found_symbol)
		found = node->tree[i];
	else
	{
		found = HAL_NewNode();
		found->symbol = static_cast<unsigned short>(symbol);
		HAL_AddNode(node, found, i);
	}

	return found;
}

void HAL_AddNode(HAL_TREE* tree, HAL_TREE* node, int position)
{
	// this function attachs a new child node to the sub-tree of the tree specified

	// allocate room for one more child node, which may mean allocating the sub-tree from scratch
	if (tree->tree == nullptr)
		tree->tree = static_cast<HAL_TREE**>(std::malloc(sizeof(HAL_TREE*) * (tree->branch + 1)));
	else
	{
		tree->tree = static_cast<HAL_TREE**>(std::realloc(tree->tree, sizeof(HAL_TREE*) * (tree->branch + 1)));
	}

	if (tree->tree == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddNode() unable to reallocate subtree\n");

	// shuffle nodes down so that we can insert new node at subtree index given by position
	for (int i = tree->branch; i > position; --i)
		tree->tree[i] = tree->tree[i - 1];

	// add the new node to the sub-tree
	tree->tree[position] = node;
	tree->branch++;
}

int HAL_SearchNode(HAL_TREE* node, int symbol, bool* found_symbol)
{
	// this function performs a binary search for the specified symbol on the subtree of the
	// given node. Return the position of the child node in the subtree if the symbol was found,
	// or the position where it should be inserted to keep the subtree sorted if it wasn't

	// handle the special case where the subtree is empty
	if (node->branch == 0)
	{
		*found_symbol = false;
		return 0;
	}

	// perform a binary search on the subtree
	int imin = 0;
	int imax = node->branch - 1;

	while (true)
	{
		const int middle = (imin + imax) / 2;
		const int compar = symbol - node->tree[middle]->symbol;

		if (compar == 0)
		{
			*found_symbol = true;
			return middle;
		}
		else if (compar > 0)
		{
			if (imax == middle)
			{
				*found_symbol = false;
				return middle + 1;
			}

			imin = middle + 1;
		}
		else
		{
			if (imin == middle)
			{
				*found_symbol = false;
				return middle;
			}

			imax = middle - 1;
		}
	}
}

void HAL_InitializeContext(HAL_MODEL* model)
{
	// this function sets the context of the model to a default value

	for (int i = 0; i <= model->order; ++i)
		model->context[i] = nullptr; // reset all the context elements
}

void HAL_Learn(HAL_MODEL* model, HAL_DICTIONARY* words)
{
	// this function learns from the user's input

	int i;
	unsigned short symbol;

	if (words->size <= model->order)
		return; // only learn from inputs which are long enough

	// train the model in the forward direction. Start by initializing the context of the model
	HAL_InitializeContext(model);
	model->context[0] = model->forward;

	for (i = 0; i < static_cast<int>(words->size); ++i)
	{
		// add the symbol to the model's dictionary if necessary, and update the model accordingly
		symbol = HAL_AddWord(model->dictionary, words->entry[i]);
		HAL_UpdateModel(model, symbol);
	}

	// add the sentence-terminating symbol
	HAL_UpdateModel(model, 1);

	// train the model in the backwards direction. Start by initializing the context of the model
	HAL_InitializeContext(model);
	model->context[0] = model->backward;

	for (i = words->size - 1; i >= 0; --i)
	{
		// find the symbol in the model's dictionary, and update the backward model accordingly
		symbol = HAL_FindWord(model->dictionary, words->entry[i]);
		HAL_UpdateModel(model, symbol);
	}

	// add the sentence-terminating symbol
	HAL_UpdateModel(model, 1);

	return;
}

void HAL_SaveTree(std::FILE* file, HAL_TREE* node)
{
	// this function saves a tree structure to the specified file

	if (node)
	{
		std::fwrite(&node->symbol, sizeof(unsigned short), 1, file);
		std::fwrite(&node->usage, sizeof(unsigned long), 1, file);
		std::fwrite(&node->count, sizeof(unsigned short), 1, file);
		std::fwrite(&node->branch, sizeof(unsigned short), 1, file);

		for (int i = 0; i < node->branch; ++i)
			HAL_SaveTree(file, node->tree[i]);
	}
}

void HAL_LoadTree(std::FILE* file, HAL_TREE* node)
{
	// this function loads a tree structure from the specified file

	std::fread(&node->symbol, sizeof(unsigned short), 1, file);
	std::fread(&node->usage, sizeof(unsigned long), 1, file);
	std::fread(&node->count, sizeof(unsigned short), 1, file);
	std::fread(&node->branch, sizeof(unsigned short), 1, file);

	if (node->branch == 0)
		return; // reliability check

	node->tree = static_cast<HAL_TREE**>(std::malloc(sizeof(HAL_TREE*) * node->branch));
	if (node->tree == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_LoadTree() unable to allocate subtree\n");

	for (int i = 0; i < node->branch; ++i)
	{
		node->tree[i] = HAL_NewNode();
		HAL_LoadTree(file, node->tree[i]);
	}
}

void HAL_MakeWords(char* input, HAL_DICTIONARY* words)
{
	// this function breaks a string into an array of words
	int offset = 0;

	if (!input || !*input)
		return; // if void, return

	// re-written

	size_t iLen = std::strlen(input);

	// clear the entries in the dictionary
	HAL_EmptyDictionary(words);

	// loop forever
	while (true)
	{
		// if the current character is of the same type as the previous character, then include
		// it in the word. Otherwise, terminate the current word.
		if (HAL_BoundaryExists(input, offset))
		{
			// add the word to the dictionary
			if (words->entry == nullptr)
				words->entry = static_cast<HAL_STRING*>(std::malloc((words->size + 1) * sizeof(HAL_STRING)));
			else
				words->entry = static_cast<HAL_STRING*>(std::realloc(words->entry, (words->size + 1) * sizeof(HAL_STRING)));

			if (words->entry == nullptr)
				BotMessage(nullptr, 1, "HAL_MakeWords() unable to reallocate dictionary\n");

			words->entry[words->size].length = static_cast<unsigned char>(offset);
			words->entry[words->size].word = input;
			words->size++;

			if (offset == static_cast<int>(std::strlen(input)))
				break;

			input += offset;
			offset = 0;
		}
		else
			offset++;
	}
	return; // finished, no need to add punctuation (it's an ACTION game, woohoo!)
}

/*
void HAL_MakeWords (char *input, HAL_DICTIONARY *words)
{
   // this function breaks a string into an array of words
   int iLen;
   int iStart;
   int iEnd;

   char *szNewWord;
   int iNewWordLength;

   HAL_STRING *pNewWord;

   //int offset = 0;

   // clear the entries in the dictionary
   HAL_EmptyDictionary (words);

   if ( !input || !*input )
	  return; // if void, return

   // re-written

   iLen = std::strlen(input);
   iStart = 0;
   iEnd = 0;

   while ( iStart < iLen )
   {
	   if ( HAL_BoundaryExists(input,iEnd) || (iEnd == iLen) )
		   // If there is a new word to take in or at the end of the string
		   // then add the word to the dictionary
	   {
		   // add the word to the dictionary
		   if (words->entry == NULL)
			   words->entry = (HAL_STRING *) malloc ((words->size + 1) * sizeof (HAL_STRING));
		   else
			   words->entry = (HAL_STRING *) realloc (words->entry,  (words->size + 1) * sizeof (HAL_STRING));

		   // get pointer to new word for quick access
		   pNewWord = &words->entry[words->size];
		   // work out new word length for new string
		   iNewWordLength = iEnd - iStart;

		   szNewWord = (char*)std::malloc(sizeof(char)*(iNewWordLength+1));

		   // copy word into string szNewWord
		   // word starts from position iStart until iEnd
		   // (goes on for iNewWordLength)
		   std::strncpy(szNewWord,&input[iStart],iNewWordLength);
		   szNewWord[iNewWordLength] = 0;

		   // update new word store
		   pNewWord->length = iNewWordLength;
		   pNewWord->word = szNewWord;

		   // increment number of words stored
		   words->size++;

		   iStart = iEnd;
	   }

	   iEnd++;
   }
}*/

bool HAL_BoundaryExists(const char* string, int position)
{
	// this function returns whether or not a word boundary exists in a string at the
	// specified location

	if (position == 0)
		return false;

	if (position == static_cast<int>(std::strlen(string)))
		return true;

	if (string[position] == '\'' && isalpha(string[position - 1]) != 0
		&& isalpha(string[position + 1]) != 0)
		return false;

	if (position > 1 && string[position - 1] == '\''
		&& isalpha(string[position - 2]) != 0 && isalpha(string[position]) != 0)
		return false;

	if (isalpha(string[position]) != 0 && isalpha(string[position - 1]) == 0)
		return true;

	if (isalpha(string[position]) == 0 && isalpha(string[position - 1]) != 0)
		return true;

	if (isdigit(string[position]) != isdigit(string[position - 1]))
		return true;

	return false;
}

bool HAL_DictionariesDiffer(HAL_DICTIONARY* words1, HAL_DICTIONARY* words2)
{
	// this function returns true if the dictionaries are NOT the same or false if not

	if (words1->size != words2->size)
		return true; // if they haven't the same size, obviously they aren't the same

	// for each word of the first dictionary...
	for (int i = 0; i < static_cast<int>(words1->size); ++i)
		if (HAL_CompareWords(words1->entry[i], words2->entry[i]) != 0)
			return true; // compare it with the second and break at the first difference

	return false; // looks like those dictionaries are identical
}

HAL_DICTIONARY* BotHALMakeKeywords(CBot* pBot, HAL_DICTIONARY* words)
{
	// this function puts all the interesting words from the user's input into a keywords
	// dictionary, which will be used when generating a reply

	static HAL_DICTIONARY* keys = nullptr;
	int i;
	int j;
	int c;

	if (!pBot)
		return nullptr;
	if (!pBot->IsUsed())
		return nullptr;

	if (keys == nullptr)
		keys = HAL_NewDictionary();

	for (i = 0; i < static_cast<int>(keys->size); ++i)
	{
		//		if (keys->entry[i].word != NULL)
		std::free(keys->entry[i].word);

		keys->entry[i].word = nullptr;
	}

	HAL_EmptyDictionary(keys);

	for (i = 0; i < static_cast<int>(words->size); ++i)
	{
		// find the symbol ID of the word. If it doesn't exist in the model, or if it begins
		// with a non-alphanumeric character, or if it is in the exclusion array, then skip it

		c = 0;

		for (j = 0; j < pBot->m_Profile.m_HAL->swappable_keywords->size; ++j)
			if (HAL_CompareWords(pBot->m_Profile.m_HAL->swappable_keywords->from[j], words->entry[i]) == 0)
			{
				BotHALAddKeyword(pBot, keys, pBot->m_Profile.m_HAL->swappable_keywords->to[j]);
				++c;
			}

		if (c == 0)
			BotHALAddKeyword(pBot, keys, words->entry[i]);
	}

	if (keys->size > 0)
		for (i = 0; i < static_cast<int>(words->size); ++i)
		{
			c = 0;

			for (j = 0; j < pBot->m_Profile.m_HAL->swappable_keywords->size; ++j)
				if (HAL_CompareWords(pBot->m_Profile.m_HAL->swappable_keywords->from[j], words->entry[i]) == 0)
				{
					BotHALAddAuxiliaryKeyword(pBot, keys, pBot->m_Profile.m_HAL->swappable_keywords->to[j]);
					++c;
				}

			if (c == 0)
				BotHALAddAuxiliaryKeyword(pBot, keys, words->entry[i]);
		}

	return keys;
}

int strpos(char* pos, char* start)
{
	return int(pos) - int(start);
}

void FillStringArea(char* string, int maxstring, char* fill, int maxfill, int start, int end)
{
	const int size = sizeof(char) * (maxstring + 1);

	char* before = static_cast<char*>(std::malloc(size));
	char* after = static_cast<char*>(std::malloc(size));

	std::memset(before, 0, size);
	std::memset(after, 0, size);

	std::strncpy(before, string, start);
	std::strncpy(after, &string[end], maxstring - end);

	snprintf(string, sizeof(string), "%s%s%s", before, fill, after);

	//	if (before != NULL)
	std::free(before);

	before = nullptr;

	//	if (after != NULL)
	std::free(after);

	after = nullptr;
}

void BotHALAddKeyword(CBot* pBot, HAL_DICTIONARY* keys, HAL_STRING word)
{
	// this function adds a word to the keyword dictionary

	if (!pBot || !pBot->IsUsed())
		return; // reliability check

	int symbol = HAL_FindWord(pBot->m_Profile.m_HAL->bot_model->dictionary, word);

	if (symbol == 0)
		return; // if void, return

	if (isalnum(word.word[0]) == 0)
		return; // if not alphanumeric, return

	symbol = HAL_FindWord(pBot->m_Profile.m_HAL->banned_keywords, word); // is this word in the banned dictionary ?

	if (symbol != 0)
		return; // if so, return

	/*if ( word.word )
	{
		char *pName = strdup(pBot->m_szBotName);

		if ( pName )
		{
			int len;

			RemoveNameTags(pBot->m_szBotName,pName);

			len = std::strlen(pName)+3;

			char *pCheck = new char[len];

			if ( pCheck )
			{
				char *strsearch;
				int namelen;
				int wordlen;
				char *theword;

				int start;
				int end;

				theword = word.word;

				std::sprintf(pCheck,"%s",pName);

				strlow(pCheck);

				namelen = std::strlen(pName);
				wordlen = std::strlen(theword);

				strsearch = theword;

				while ( (strsearch = std::strstr(strsearch,pCheck)) != NULL )
				{
					start = strpos(strsearch,theword);
					end = (int)start+namelen;

					if ( start == 0 )
					{
						if ( end < wordlen )
						{
							if ( !isalnum(theword[end]) )
								FillStringArea(theword,wordlen,"%n",2,start,end);
						}
						else
							FillStringArea(theword,wordlen,"%n",2,start,end);
					}
					else
					{
						if ( end < wordlen )
						{
							if ( !isalnum(theword[start-1]) && !isalnum(theword[end]) )
								FillStringArea(theword,wordlen,"%n",2,start,end);
						}
						else
							FillStringArea(theword,wordlen,"%n",2,start,end);
					}
				}

				// name match, don't use name as keyword?
				if ( std::strstr(word.word,pCheck) != NULL )
				{
					std::free(pCheck);
					pCheck = NULL;
					return;
				}
				else if ( std::strncmp(word.word,&pCheck[1],len-2) == 0 )
				{
					std::free(pCheck);
					pCheck = NULL;
					return;
				}
				std::free(pCheck);
				pCheck = NULL;
			}
			std::free(pName);
			pName = NULL;
		}
	}
	*/

	symbol = HAL_FindWord(pBot->m_Profile.m_HAL->auxiliary_keywords, word); // is this word in the auxiliary dictionary ?

	if (symbol != 0)
		return; // if so, return

	HAL_AddWord(keys, word); // once we are sure this word isn't known yet, we can add it
}

void BotHALAddAuxiliaryKeyword(CBot* pBot, HAL_DICTIONARY* keys, HAL_STRING word)
{
	// this function adds an auxilliary keyword to the keyword dictionary

	if (!pBot || !pBot->IsUsed())
		return; // reliability check

	int symbol = HAL_FindWord(pBot->m_Profile.m_HAL->bot_model->dictionary, word);

	if (symbol == 0)
		return; // if void, return

	if (isalnum(word.word[0]) == 0)
		return; // if not alphanumeric, return

	symbol = HAL_FindWord(pBot->m_Profile.m_HAL->auxiliary_keywords, word); // is it already in the dictionary ?
	if (symbol == 0)
		return; // if already in dictionary, return

	HAL_AddWord(keys, word); // add this word to the keywords dictionary
}

HAL_DICTIONARY* BotHALBuildReplyDictionary(CBot* pBot, HAL_DICTIONARY* keys)
{
	// this function generates a dictionary of reply words relevant to the dictionary of keywords

	static HAL_DICTIONARY* replies = nullptr;
	int i;
	int symbol;
	bool start = true;

	if (!pBot || !pBot->IsUsed())
		return nullptr; // reliability check

	if (replies == nullptr)
		replies = HAL_NewDictionary();

	HAL_EmptyDictionary(replies);

	// start off by making sure that the model's context is empty
	HAL_InitializeContext(pBot->m_Profile.m_HAL->bot_model);
	pBot->m_Profile.m_HAL->bot_model->context[0] = pBot->m_Profile.m_HAL->bot_model->forward;
	pBot->m_Profile.m_HAL->keyword_is_used = false;

	// generate the reply in the forward direction
	while (true)
	{
		// get a random symbol from the current context
		if (start == 1)
			symbol = BotHALSeedReply(pBot, keys);
		else
			symbol = BotHALBabble(pBot, keys, replies);

		if (symbol == 0 || symbol == 1)
			break;

		start = false;

		// append the symbol to the reply dictionary
		if (replies->entry == nullptr)
			replies->entry = static_cast<HAL_STRING*>(std::malloc((replies->size + 1) * sizeof(HAL_STRING)));
		else
			replies->entry = static_cast<HAL_STRING*>(std::realloc(replies->entry, (replies->size + 1) * sizeof(HAL_STRING)));

		if (replies->entry == nullptr)
			BotMessage(nullptr, 1, "HAL: BotHALBuildReplyDictionary() unable to reallocate dictionary\n");

		replies->entry[replies->size].length = pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol].length;
		replies->entry[replies->size].word = pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol].word;
		replies->size++;

		// extend the current context of the model with the current symbol
		HAL_UpdateContext(pBot->m_Profile.m_HAL->bot_model, symbol);
	}

	// start off by making sure that the model's context is empty
	HAL_InitializeContext(pBot->m_Profile.m_HAL->bot_model);
	pBot->m_Profile.m_HAL->bot_model->context[0] = pBot->m_Profile.m_HAL->bot_model->backward;

	// re-create the context of the model from the current reply dictionary so that we can
	// generate backwards to reach the beginning of the string.
	if (replies->size > 0)
		for (i = static_cast<int>(std::fmin(replies->size - 1, pBot->m_Profile.m_HAL->bot_model->order)); i >= 0; --i)
		{
			symbol = HAL_FindWord(pBot->m_Profile.m_HAL->bot_model->dictionary, replies->entry[i]);
			HAL_UpdateContext(pBot->m_Profile.m_HAL->bot_model, symbol);
		}

	// generate the reply in the backward direction
	while (true)
	{
		// get a random symbol from the current context
		symbol = BotHALBabble(pBot, keys, replies);
		if (symbol == 0 || symbol == 1)
			break;

		// prepend the symbol to the reply dictionary
		if (replies->entry == nullptr)
			replies->entry = static_cast<HAL_STRING*>(std::malloc((replies->size + 1) * sizeof(HAL_STRING)));
		else
			replies->entry = static_cast<HAL_STRING*>(std::realloc(replies->entry, (replies->size + 1) * sizeof(HAL_STRING)));

		if (replies->entry == nullptr)
			BotMessage(nullptr, 1, "HAL: BotHALBuildReplyDictionary() unable to reallocate dictionary\n");

		// shuffle everything up for the prepend
		for (i = replies->size; i > 0; --i)
		{
			replies->entry[i].length = replies->entry[i - 1].length;
			replies->entry[i].word = replies->entry[i - 1].word;
		}

		replies->entry[0].length = pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol].length;
		replies->entry[0].word = pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol].word;
		replies->size++;

		// extend the current context of the model with the current symbol
		HAL_UpdateContext(pBot->m_Profile.m_HAL->bot_model, symbol);
	}

	return replies;
}

int BotHALBabble(CBot* pBot, HAL_DICTIONARY* keys, HAL_DICTIONARY* words)
{
	// this function returns a random symbol from the current context, or a zero symbol
	// identifier if we've reached either the start or end of the sentence. Selection of the
	// symbol is based on probabilities, favouring keywords. In all cases, use the longest
	// available context to choose the symbol

	const HAL_TREE* node = nullptr;
	int i;
	int symbol = 0;

	if (!pBot || !pBot->IsUsed())
		return 0; // reliability check

	// select the longest available context
	for (i = 0; i <= pBot->m_Profile.m_HAL->bot_model->order; ++i)
		if (pBot->m_Profile.m_HAL->bot_model->context[i] != nullptr)
			node = pBot->m_Profile.m_HAL->bot_model->context[i];

	if (node->branch == 0)
		return 0;

	// choose a symbol at random from this context
	i = RANDOM_LONG(0, node->branch - 1);
	int count = RANDOM_LONG(0, node->usage - 1);

	while (count >= 0)
	{
		// if the symbol occurs as a keyword, then use it. Only use an auxilliary keyword if
		// a normal keyword has already been used.
		symbol = node->tree[i]->symbol;

		if (HAL_FindWord(keys, pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol]) != 0
			&& (pBot->m_Profile.m_HAL->keyword_is_used || HAL_FindWord(pBot->m_Profile.m_HAL->auxiliary_keywords, pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol]) == 0)
			&& !HAL_WordExists(words, pBot->m_Profile.m_HAL->bot_model->dictionary->entry[symbol]))
		{
			pBot->m_Profile.m_HAL->keyword_is_used = true;
			break;
		}

		count -= node->tree[i]->count;

		i = i >= node->branch - 1 ? 0 : i + 1;
	}

	return symbol;
}

bool HAL_WordExists(HAL_DICTIONARY* dictionary, HAL_STRING word)
{
	// here's a silly brute-force searcher for the reply string

	// for each element of the dictionary, compare word with it...
	for (int i = 0; i < static_cast<int>(dictionary->size); ++i)
		if (HAL_CompareWords(dictionary->entry[i], word) == 0)
			return true; // word was found

	return false; // word was not found
}

int BotHALSeedReply(CBot* pBot, HAL_DICTIONARY* keys)
{
	// this function seeds the reply by guaranteeing that it contains a keyword, if one exists

	int symbol;

	// be aware of the special case where the tree is empty
	if (pBot->m_Profile.m_HAL->bot_model->context[0]->branch == 0)
		symbol = 0;
	else
		symbol = pBot->m_Profile.m_HAL->bot_model->context[0]->tree[RANDOM_LONG(0, pBot->m_Profile.m_HAL->bot_model->context[0]->branch - 1)]->symbol;

	if (keys->size > 0)
	{
		int i = RANDOM_LONG(0, keys->size - 1);
		const int stop = i;

		while (true)
		{
			if (HAL_FindWord(pBot->m_Profile.m_HAL->bot_model->dictionary, keys->entry[i]) != 0 && HAL_FindWord(pBot->m_Profile.m_HAL->auxiliary_keywords, keys->entry[i]) == 0)
			{
				symbol = HAL_FindWord(pBot->m_Profile.m_HAL->bot_model->dictionary, keys->entry[i]);
				return symbol;
			}

			i++;

			if (i == static_cast<int>(keys->size))
				i = 0;

			if (i == stop)
				return symbol;
		}
	}

	return symbol;
}

HAL_SWAP* HAL_NewSwap()
{
	// allocate a new swap structure.

	HAL_SWAP* list = static_cast<HAL_SWAP*>(std::malloc(sizeof(HAL_SWAP)));
	if (list == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_NewSwap() unable to allocate swap\n");

	list->size = 0; // initialize to defaults
	list->from = nullptr;
	list->to = nullptr;

	return list; // return the fresh new swap
}

void HAL_AddSwap(HAL_SWAP* list, char* s, char* d)
{
	// this function adds a new entry to the swap structure.

	if (list->from == nullptr)
	{
		list->from = static_cast<HAL_STRING*>(std::malloc(sizeof(HAL_STRING)));
		if (list->from == nullptr)
			BotMessage(nullptr, 1, "HAL: HAL_AddSwap() unable to allocate list->from\n");
	}

	if (list->to == nullptr)
	{
		list->to = static_cast<HAL_STRING*>(std::malloc(sizeof(HAL_STRING)));
		if (list->to == nullptr)
			BotMessage(nullptr, 1, "HAL: HAL_AddSwap() unable to allocate list->to\n");
	}

	list->from = static_cast<HAL_STRING*>(std::realloc(list->from, sizeof(HAL_STRING) * (list->size + 1)));
	if (list->from == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddSwap() unable to reallocate from\n");

	list->to = static_cast<HAL_STRING*>(std::realloc(list->to, sizeof(HAL_STRING) * (list->size + 1)));
	if (list->to == nullptr)
		BotMessage(nullptr, 1, "HAL: HAL_AddSwap() unable to reallocate to\n");

	list->from[list->size].length = static_cast<unsigned char>(std::strlen(s));
	list->from[list->size].word = strdup(s);
	list->to[list->size].length = static_cast<unsigned char>(std::strlen(d));
	list->to[list->size].word = strdup(d);
	list->size++;
}

HAL_SWAP* HAL_InitializeSwap(const char* filename)
{
	// this function reads a swap structure from a file.

	HAL_SWAP* list = HAL_NewSwap();

	if (filename == nullptr)
		return list;

	std::FILE* fp = std::fopen(filename, "r");
	if (fp == nullptr)
		return list;

	while (!std::feof(fp))
	{
		char buffer[1024];

		if (std::fgets(buffer, 1024, fp) == nullptr)
			break;

		if (buffer[0] == '#' || buffer[0] == '\n')
			continue;

		char* from = std::strtok(buffer, "\t");
		char* to = std::strtok(nullptr, "\t\n#");

		HAL_AddSwap(list, from, to);
	}

	std::fclose(fp);
	return list;
}

HAL_DICTIONARY* HAL_InitializeList(const char* filename)
{
	// this function reads a dictionary from a file

	HAL_DICTIONARY* list = HAL_NewDictionary();

	if (filename == nullptr)
		return list;

	std::FILE* fp = std::fopen(filename, "r");
	if (fp == nullptr)
		return list;

	while (!std::feof(fp))
	{
		char buffer[1024];

		if (std::fgets(buffer, 1024, fp) == nullptr)
			break;

		if (buffer[0] == '#' || buffer[0] == '\n')
			continue;

		const char* string = std::strtok(buffer, "\t\n#");

		if (string != nullptr && string[0] != '\0')
		{
			HAL_STRING word;

			word.length = static_cast<unsigned char>(std::strlen(string));
			word.word = strdup(buffer); // strdup - duplicates string
			HAL_AddWord(list, word);
		}
	}

	std::fclose(fp);
	return list;
}

void HAL_EmptyDictionary(HAL_DICTIONARY* dictionary)
{
	// this function empties the memory space used by a dictionary, cutting it down to zero.
	// NOTE THAT IT DOES NOT FREE THE DICTIONARY MEMORY SPACE

	if (dictionary == nullptr)
		return; // reliability check

	if (dictionary->entry != nullptr)
	{
		try
		{
			std::free(dictionary->entry);
		}

		catch (...)
		{
			BotMessage(nullptr, 0, "exception freeing MegaHAL dictionary entry (Argh!!) Not freeing\n");
		}

		dictionary->entry = nullptr;
	}

	if (dictionary->index != nullptr)
	{
		try
		{
			std::free(dictionary->index);
		}

		catch (...)
		{
			BotMessage(nullptr, 0, "exception freeing MegaHAL dictionary index (Argh!!) Not freeing\n");
		}

		dictionary->index = nullptr;
	}

	dictionary->size = 0;
}

void HAL_FreeModel(HAL_MODEL* model)
{
	// this function frees the memory space used by a model

	if (model == nullptr)
		return; // reliability check

	if (model->forward != nullptr)
		HAL_FreeTree(model->forward);

	model->forward = nullptr;

	if (model->backward != nullptr)
		HAL_FreeTree(model->backward);

	model->backward = nullptr;

	//	if (model->context != NULL)
	std::free(model->context);

	model->context = nullptr;

	if (model->dictionary != nullptr)
	{
		HAL_EmptyDictionary(model->dictionary);
		std::free(model->dictionary);
	}

	model->dictionary = nullptr;

	//	if (model != NULL)
	std::free(model);

	model = nullptr;
}

void HAL_FreeTree(HAL_TREE* tree)
{
	// this function frees the memory space used by a model tree

	if (tree == nullptr)
		return; // reliability check

	if (tree->tree != nullptr)
	{
		for (int i = 0; i < tree->branch; ++i)
			HAL_FreeTree(tree->tree[i]);

		std::free(tree->tree);
		tree->tree = nullptr;
	}

	std::free(tree);

	tree = nullptr;
}

void HAL_FreeSwap(HAL_SWAP* swap)
{
	// this function frees the memory space used in a swap structure

	if (swap == nullptr)
		return; // if already freed, return

	// for each element of the swap structure...
	for (int i = 0; i < swap->size; ++i)
	{
		//		if (swap->from[i].word != NULL)
		std::free(swap->from[i].word); // free the "from" word

		swap->from[i].word = nullptr;

		// free TO

//		if (swap->to[i].word != NULL)
		std::free(swap->to[i].word); // free the "to" word

		swap->to[i].word = nullptr;
	}

	//	if (swap->from != NULL)
	std::free(swap->from); // free the "from" array

	swap->from = nullptr;

	//	if (swap->to != NULL)
	std::free(swap->to); // free the "to" array

	swap->to = nullptr;

	std::free(swap); // free the swap structure itself
	swap = nullptr;
}

// return true if need to pre-train
bool PrepareHALBrainForPersonality(bot_profile_t* pBotProfile)
{
	// this function prepares a HAL personality

	char ban_filename[256];
	char aux_filename[256];
	char swp_filename[256];
	char brn_filename[256];

	pBotProfile->m_HAL = static_cast<HAL_bot_t*>(std::malloc(sizeof(HAL_bot_t)));

	if (pBotProfile->m_HAL == nullptr)
		return false; // reliability check

	pBotProfile->m_HAL->auxiliary_keywords = nullptr;
	pBotProfile->m_HAL->banned_keywords = nullptr;
	pBotProfile->m_HAL->bot_model = nullptr;
	//   pBotProfile->m_HAL->bot_words = NULL;
	pBotProfile->m_HAL->input_words = nullptr;
	pBotProfile->m_HAL->swappable_keywords = nullptr;
	pBotProfile->m_HAL->keyword_is_used = false;

	pBotProfile->m_HAL->bot_model = HAL_NewModel(BOT_HAL_MODEL_ORDER); // create a language model of a certain order

	// build the file names

	UTIL_BuildFileName(ban_filename, BOT_PROFILES_FOLDER, nullptr);
	UTIL_BuildFileName(aux_filename, BOT_PROFILES_FOLDER, nullptr);
	UTIL_BuildFileName(swp_filename, BOT_PROFILES_FOLDER, nullptr);

	// sprintf (ban_filename, BOT_PROFILES_FOLDER);
	// sprintf (aux_filename, BOT_PROFILES_FOLDER);
	// sprintf (swp_filename, BOT_PROFILES_FOLDER);

	char brn_file[256];

	snprintf(brn_file, sizeof(brn_file), "%d_hal.brn", pBotProfile->m_iProfileId);
	UTIL_BuildFileName(brn_filename, BOT_PROFILES_FOLDER, brn_file);

	//#ifdef __linux__
	  // sprintf (brn_filename, "%s%d_hal.brn", BOT_PROFILES_FOLDER, pBotProfile->m_iProfileId);
	//#else
	   //sprintf (brn_filename, "%s%d_hal.brn", BOT_PROFILES_FOLDER, pBotProfile->m_iProfileId);
	//#endif

	if (pBotProfile->m_szHAL_BanFile)
		std::strcat(ban_filename, pBotProfile->m_szHAL_BanFile);
	else
	{
		char szFilename[16];

		snprintf(szFilename, sizeof(szFilename), "%d_hal.ban", pBotProfile->m_iProfileId);
		std::strcat(ban_filename, szFilename);
	}

	if (pBotProfile->m_szHAL_AuxFile)
		std::strcat(aux_filename, pBotProfile->m_szHAL_AuxFile);
	else
	{
		char szFilename[16];

		snprintf(szFilename, sizeof(szFilename), "%d_hal.aux", pBotProfile->m_iProfileId);
		std::strcat(aux_filename, szFilename);
	}

	if (pBotProfile->m_szHAL_SwapFile)
		std::strcat(swp_filename, pBotProfile->m_szHAL_SwapFile);
	else
	{
		char szFilename[16];

		snprintf(szFilename, sizeof(szFilename), "%d_hal.swp", pBotProfile->m_iProfileId);
		std::strcat(swp_filename, szFilename);
	}

	// read dictionaries containing banned keywords, auxiliary keywords and swap keywords
	pBotProfile->m_HAL->banned_keywords = HAL_InitializeList(ban_filename);
	pBotProfile->m_HAL->auxiliary_keywords = HAL_InitializeList(aux_filename);
	pBotProfile->m_HAL->swappable_keywords = HAL_InitializeSwap(swp_filename);

	// check if the brain exists, try to open it
	std::FILE* fp = std::fopen(brn_filename, "rb");
	if (fp != nullptr)
	{
		char cookie[32];

		std::fseek(fp, 0, SEEK_SET); // seek at start of file
		std::fread(cookie, sizeof"RCBOTHAL", 1, fp); // read the brain signature
		std::fclose(fp); // close the brain (we just wanted the signature)

		// check for brain file validity
		if (std::strcmp(cookie, "RCBOTHAL") == 0)
			return false; // ok, brain is valid
	}

	// there is a problem with the brain, infer a brand new one
	BotMessage(nullptr, 0, "bot profile (%d) HAL brain damaged!", pBotProfile->m_iProfileId);
	BotMessage(nullptr, 0, "inferring a new HAL brain to profile (%d)", pBotProfile->m_iProfileId);

	// create the new brain (i.e, save a void one in the brain file)
	fp = std::fopen(brn_filename, "wb");

	if (fp == nullptr)
	{
		BotMessage(nullptr, 1, "PrepareHALBrainForPersonality(): writing permissions not allowed on profile (%d) HAL brain!", pBotProfile->m_iProfileId);
		return false; // return or exit here [APG]RoboCop[CL]
	}

	std::fwrite("RCBOTHAL", sizeof"RCBOTHAL", 1, fp);
	std::fwrite(&pBotProfile->m_HAL->bot_model->order, sizeof(unsigned char), 1, fp);
	HAL_SaveTree(fp, pBotProfile->m_HAL->bot_model->forward);
	HAL_SaveTree(fp, pBotProfile->m_HAL->bot_model->backward);
	HAL_SaveDictionary(fp, pBotProfile->m_HAL->bot_model->dictionary);
	std::fclose(fp); // everything is saved, close the file

	return true; // ok, now it is guarantee that this personality has an associated brain
}

bool LoadHALBrainForPersonality(const bot_profile_t* pBotProfile, bool bPreTrain)
{
	// this function loads a HAL brain

	char filename[512];
	char file[256];
	char cookie[sizeof"RCBOTHAL"];

	//int iNameLength;
	//char *szName;

	//iNameLength = std::strlen(pBotProfile->m_szBotName);
	//szName = new char [ sizeof(char) * (iNameLength + 1) ];
	//RemoveNameTags(pBotProfile->m_szBotName,szName);
	//szName[iNameLength] = 0;

	snprintf(file, sizeof(file), "%d_hal.brn", pBotProfile->m_iProfileId);

	UTIL_BuildFileName(filename, "botprofiles", file);

	// check if the brain exists, try to open it
	std::FILE* fp = std::fopen(filename, "rb");
	if (fp == nullptr)
	{
		//delete szName;
		BotMessage(nullptr, 0, "LoadHALBrainForPersonality(): %s's HAL brain refuses to wake up! (file does not exist)", pBotProfile->m_szBotName);
		return true; // there was an error, return true
	}

	// check for brain file validity
	std::fread(cookie, sizeof"RCBOTHAL", 1, fp); // read the brain signature

	if (std::strcmp(cookie, "RCBOTHAL") != 0)
	{
		// delete szName;
		BotMessage(nullptr, 0, "LoadHALBrainForPersonality(): %s's HAL brain damaged!", pBotProfile->m_szBotName); // bad brain
		BotMessage(nullptr, 2, "damaged bot file, delete %s/%d.brn file", BOT_PROFILES_FOLDER, pBotProfile->m_iProfileId); // bad brain
		std::fclose(fp); // close file
		return true; // there was an error, return true
	}

	BotMessage(nullptr, 0, "HAL : restoring brain to %s\n", pBotProfile->m_szBotName);

	std::fread(&pBotProfile->m_HAL->bot_model->order, 1, 1, fp);
	HAL_LoadTree(fp, pBotProfile->m_HAL->bot_model->forward);
	HAL_LoadTree(fp, pBotProfile->m_HAL->bot_model->backward);
	HAL_LoadDictionary(fp, pBotProfile->m_HAL->bot_model->dictionary);
	std::fclose(fp);

	pBotProfile->m_HAL->input_words = HAL_NewDictionary(); // create the global chat dictionary

	pBotProfile->m_HAL->keyword_is_used = false;

	//HAL_AddSwap(pBotProfile->m_HAL->swappable_keywords,szName,name_in_msg);

	if (bPreTrain)
	{
		char trn_filename[256];

		UTIL_BuildFileName(trn_filename, BOT_PROFILES_FOLDER);

		if (pBotProfile->m_szHAL_PreTrainFile)
		{
			std::strcat(trn_filename, "/");
			std::strcat(trn_filename, pBotProfile->m_szHAL_PreTrainFile);
		}
		else
		{
			char szFilename[16];

			snprintf(szFilename, sizeof(szFilename), "%d_hal.trn", pBotProfile->m_iProfileId);
			std::strcat(trn_filename, szFilename);
		}

		// see if there is a training file
		fp = std::fopen(trn_filename, "r");

		if (fp != nullptr)
		{
			char szBuffer[256];

			while (std::fgets(szBuffer, 255, fp) != nullptr)
			{
				szBuffer[255] = 0;

				if (szBuffer[0] == '#')
					continue; // a comment
				if (!szBuffer[0])
					continue; // nothing on this line

				const size_t iLen = std::strlen(szBuffer);
				if (iLen > 0 && szBuffer[iLen - 1] == '\n')
					szBuffer[iLen - 1] = 0;

				HAL_MakeWords(szBuffer, pBotProfile->m_HAL->input_words);
				HAL_Learn(pBotProfile->m_HAL->bot_model, pBotProfile->m_HAL->input_words);
			}

			std::fclose(fp);
		}
	}

	return false; // no error, return false
}

void SaveHALBrainForPersonality(const bot_profile_t* pBotProfile)
{
	// this function saves the current state to a HAL brain file

	char file[256];
	char filename[256];

	snprintf(file, sizeof(file), "%d_hal.brn", pBotProfile->m_iProfileId);

	UTIL_BuildFileName(filename, "botprofiles", file);

	std::FILE* fp = std::fopen(filename, "wb");
	if (fp == nullptr)
	{
		BotMessage(nullptr, 0, "Unable to save profile %d's HAL brain to %s\n", pBotProfile->m_iProfileId, filename);
		return;
	}

	// dump the HAL brain to disk
	std::fwrite("RCBOTHAL", sizeof"RCBOTHAL", 1, fp);
	std::fwrite(&pBotProfile->m_HAL->bot_model->order, sizeof(unsigned char), 1, fp);
	HAL_SaveTree(fp, pBotProfile->m_HAL->bot_model->forward);
	HAL_SaveTree(fp, pBotProfile->m_HAL->bot_model->backward);
	HAL_SaveDictionary(fp, pBotProfile->m_HAL->bot_model->dictionary);

	std::fclose(fp);
}

void FreeHALBrain(const bot_profile_t* pBotProfile)
{
	if (!pBotProfile->m_HAL)
		return;
	// free every word in their global chat dictionary
	if (pBotProfile->m_HAL->bot_model->dictionary)
	{
		for (int j = 0; j < static_cast<int>(pBotProfile->m_HAL->bot_model->dictionary->size); j++)
		{
			//			if (pBotProfile->m_HAL->bot_model->dictionary->entry[j].word != NULL)
			{
				std::free(pBotProfile->m_HAL->bot_model->dictionary->entry[j].word); // free the word
			}

			pBotProfile->m_HAL->bot_model->dictionary->entry[j].word = nullptr; // and fools the pointer
		}

		HAL_EmptyDictionary(pBotProfile->m_HAL->bot_model->dictionary); // empty that dictionary itself

		//		if (pBotProfile->m_HAL->bot_model->dictionary != NULL)
		std::free(pBotProfile->m_HAL->bot_model->dictionary); // now frees the dictionary

		pBotProfile->m_HAL->bot_model->dictionary = nullptr; // and fools the pointer
	}

	HAL_EmptyDictionary(pBotProfile->m_HAL->input_words);
	pBotProfile->m_HAL->input_words = nullptr;
	HAL_EmptyDictionary(pBotProfile->m_HAL->banned_keywords);
	pBotProfile->m_HAL->banned_keywords = nullptr;
	HAL_EmptyDictionary(pBotProfile->m_HAL->auxiliary_keywords);
	pBotProfile->m_HAL->auxiliary_keywords = nullptr;
	//HAL_EmptyDictionary (pBotProfile->m_HAL->bot_words);

	HAL_FreeModel(pBotProfile->m_HAL->bot_model);
	pBotProfile->m_HAL->bot_model = nullptr;
	HAL_FreeSwap(pBotProfile->m_HAL->swappable_keywords);
	pBotProfile->m_HAL->swappable_keywords = nullptr;
}