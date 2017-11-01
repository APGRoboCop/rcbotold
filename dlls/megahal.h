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
#ifndef _MEGAHAL_H_
#define _MEGAHAL_H_

// HAL-related structure definitions
typedef struct
{
   unsigned char length; // length of word (for quick access)
   char *word; // the string itself
} HAL_STRING;


typedef struct
{
   unsigned long size; // size of dictionary (for quick access)
   HAL_STRING *entry; // string entry at this position of the dictionary
   unsigned short *index; // pointer to index
} HAL_DICTIONARY;


typedef struct
{
   unsigned short size; // size of the word swap structure
   HAL_STRING *from; // word to be changed into the next
   HAL_STRING *to; // word for replacing the previous
} HAL_SWAP;


typedef struct HAL_NODE
{
   unsigned short symbol; // symbol ID
   unsigned long usage; // usage information
   unsigned short count; // usage count (?)
   unsigned short branch; // branch of the tree
   struct HAL_NODE **tree; // pointer to tree node pointer
} HAL_TREE;


typedef struct
{
   unsigned char order; // model order (complexity)
   HAL_TREE *forward; // pointer to the model's forward tree
   HAL_TREE *backward; // pointer to the model's backwards tree
   HAL_TREE **context; // pointer to context tree pointer
   HAL_DICTIONARY *dictionary; // pointer to the model's dictionary
} HAL_MODEL;

typedef struct
{
   HAL_DICTIONARY *banned_keywords; // dictionary of words that must never be used as keywords
   HAL_DICTIONARY *auxiliary_keywords; // dictionary of auxiliary keywords
   HAL_SWAP *swappable_keywords; // array of swappable keywords with their equivalences
   HAL_MODEL *bot_model; // Markov model of the bot
   HAL_DICTIONARY *input_words; // global chat's dictionary of words
//   HAL_DICTIONARY *bot_words; // bot's own dictionary of words
   BOOL keyword_is_used;
} HAL_bot_t;

// bot HAL Markov model order
#define BOT_HAL_MODEL_ORDER 5

#ifndef CBot
class CBot;
#endif

void HumanizeString ( char *string );
void RemoveNameTags ( const char *in_string, char *out_string );

void BotChat (CBot *pBot);
void BotSayText (CBot *pBot);
void BotSayAudio (CBot *pBot);
void BotTalk (CBot *pBot, char *sound_path);

void FreeHALBrain ( struct bot_profile_s *pBotProfile );
void HAL_LoadTree (FILE *file, HAL_TREE *node);
void HAL_LoadDictionary (FILE *file, HAL_DICTIONARY *dictionary);
void HAL_SaveTree (FILE *file, HAL_TREE *node);
void HAL_SaveDictionary (FILE *file, HAL_DICTIONARY *dictionary);
void HAL_Learn (HAL_MODEL *model, HAL_DICTIONARY *words);
unsigned short HAL_AddWord (HAL_DICTIONARY *dictionary, HAL_STRING word);
int HAL_SearchDictionary (HAL_DICTIONARY *dictionary, HAL_STRING word, BOOL *find);
unsigned short HAL_FindWord (HAL_DICTIONARY *dictionary, HAL_STRING word);
int HAL_CompareWords (HAL_STRING word1, HAL_STRING word2);
void HAL_InitializeDictionary (HAL_DICTIONARY *dictionary);
HAL_DICTIONARY *HAL_NewDictionary (void);
HAL_TREE *HAL_NewNode (void);
HAL_MODEL *HAL_NewModel (int order);
void HAL_UpdateModel (HAL_MODEL *model, int symbol);
void HAL_UpdateContext (HAL_MODEL *model, int symbol);
HAL_TREE *HAL_AddSymbol (HAL_TREE *tree, unsigned short symbol);
HAL_TREE *HAL_FindSymbol (HAL_TREE *node, int symbol);
HAL_TREE *HAL_FindSymbolAdd (HAL_TREE *node, int symbol);
void HAL_AddNode (HAL_TREE *tree, HAL_TREE *node, int position);
int HAL_SearchNode (HAL_TREE *node, int symbol, BOOL *found_symbol);
void HAL_InitializeContext (HAL_MODEL *model);
void BotHALTrainModel (CBot *pBot, HAL_MODEL *model);
void HAL_ShowDictionary (HAL_DICTIONARY *dictionary);
void HAL_MakeWords (char *input, HAL_DICTIONARY *words);
void BotHALGenerateReply (CBot *pBot, char *output);
BOOL HAL_BoundaryExists (char *string, int position);
void BotChatReply ( CBot *pBot, char *szMsg, edict_t *pSender, char *szReplyMsg );
BOOL HAL_DictionariesDiffer (HAL_DICTIONARY *words1, HAL_DICTIONARY *words2);
HAL_DICTIONARY *BotHALMakeKeywords (CBot *pBot, HAL_DICTIONARY *words);
void BotHALAddKeyword (CBot *pBot, HAL_DICTIONARY *keys, HAL_STRING word);
void BotHALAddAuxiliaryKeyword (CBot *pBot, HAL_DICTIONARY *keys, HAL_STRING word);
HAL_DICTIONARY *BotHALBuildReplyDictionary (CBot *pBot, HAL_DICTIONARY *keys);
int BotHALBabble (CBot *pBot, HAL_DICTIONARY *keys, HAL_DICTIONARY *words);
BOOL HAL_WordExists (HAL_DICTIONARY *dictionary, HAL_STRING word);
int BotHALSeedReply (CBot *pBot, HAL_DICTIONARY *keys);
HAL_SWAP *HAL_NewSwap (void);
void HAL_AddSwap (HAL_SWAP *list, char *s, char *d);
HAL_SWAP *HAL_InitializeSwap (char *filename);
HAL_DICTIONARY *HAL_InitializeList (char *filename);
void HAL_EmptyDictionary (HAL_DICTIONARY *dictionary);
void HAL_FreeModel (HAL_MODEL *model);
void HAL_FreeTree (HAL_TREE *tree);
void HAL_FreeSwap (HAL_SWAP *swap);
BOOL PrepareHALBrainForPersonality (struct bot_profile_s *pBotProfile);
BOOL LoadHALBrainForPersonality (struct bot_profile_s *pBotProfile, BOOL bPreTrain);
void SaveHALBrainForPersonality (struct bot_profile_s *pBotProfile);

#endif