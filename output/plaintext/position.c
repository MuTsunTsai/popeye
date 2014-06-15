#include "output/plaintext/position.h"
#include "output/plaintext/plaintext.h"
#include "output/plaintext/language_dependant.h"
#include "output/plaintext/condition.h"
#include "output/plaintext/pieces.h"
#include "output/plaintext/message.h"
#include "output/output.h"
#include "input/plaintext/problem.h"
#include "input/plaintext/stipulation.h"
#include "options/maxthreatlength.h"
#include "options/maxflightsquares.h"
#include "options/nontrivial.h"
#include "conditions/grid.h"
#include "conditions/imitator.h"
#include "pieces/attributes/neutral/neutral.h"
#include "pieces/walks/classification.h"
#include "pieces/walks/hunters.h"
#include "position/position.h"
#include "solving/castling.h"
#include "solving/move_generator.h"
#include "solving/proofgames.h"
#include "debugging/assert.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

enum
{
  fileWidth = 4
};

static void CenterLine(FILE *file, char const *format, ...)
{
  int nr_chars;

  {
    char dummy;
    va_list args;
    va_start(args,format);
    nr_chars = vsnprintf(&dummy,1,format,args);
    va_end(args);
  }

  {
    int const indentation = (fileWidth*nr_files_on_board>nr_chars
                             ? 3+(fileWidth*nr_files_on_board-nr_chars)/2
                             : 1);
    fprintf(file,"%*s",indentation," ");

    {
      va_list args;
      va_start(args,format);
      vfprintf(file,format,args);
      va_end(args);
    }
  }

  fputc('\n',file);
}

static void MultiCenter(FILE *file, char *s)
{
  char *start_of_line = s;
  char *end_of_line = strchr(start_of_line,'\n');

  while (end_of_line)
  {
    *end_of_line = '\0';
    CenterLine(file,"%s",start_of_line);
    *end_of_line = '\n';
    start_of_line = end_of_line+1;
    end_of_line = strchr(start_of_line,'\n');
  }
}

static boolean is_square_occupied_by_imitator(position const *pos, square s)
{
  boolean result = false;
  unsigned int imi_idx;

  for (imi_idx = 0; imi_idx<pos->number_of_imitators; ++imi_idx)
    if (s==pos->isquare[imi_idx])
    {
      result = true;
      break;
    }

  return result;
}

static void WriteCastlingMutuallyExclusive(FILE *file)
{
  /* no need to test in [Black] - information is redundant */
  if (castling_mutual_exclusive[White][queenside_castling-min_castling]!=0
      || castling_mutual_exclusive[White][kingside_castling-min_castling]!=0)
  {
    fputs(OptString[UserLanguage][mutuallyexclusivecastling],file);

    if ((castling_mutual_exclusive[White][queenside_castling-min_castling]
         &ra_cancastle))
    {
      fputc(' ',file);
      WriteSquare(file,square_a1);
      WriteSquare(file,square_a8);
    }

    if ((castling_mutual_exclusive[White][queenside_castling-min_castling]
         &rh_cancastle))
    {
      fputc(' ',file);
      WriteSquare(file,square_a1);
      WriteSquare(file,square_h8);
    }

    if ((castling_mutual_exclusive[White][kingside_castling-min_castling]
         &ra_cancastle))
    {
      fputc(' ',file);
      WriteSquare(file,square_h1);
      WriteSquare(file,square_a8);
    }

    if ((castling_mutual_exclusive[White][kingside_castling-min_castling]
         &rh_cancastle))
    {
      fputc(' ',file);
      WriteSquare(file,square_h1);
      WriteSquare(file,square_h8);
    }

    fputc('\n',file);
  }
}

static void WriteGrid(FILE *file)
{
  square square, square_a;
  int row, column;
  char    HLine[40];

  static char BorderL[]="+---a---b---c---d---e---f---g---h---+\n";
  static char HorizL[]="%c                                   %c\n";
  static char BlankL[]="|                                   |\n";

  fputc('\n',file);
  fputs(BorderL,file);
  fputs(BlankL,file);

  for (row=0, square_a = square_a8;
       row<nr_rows_on_board;
       row++, square_a += dir_down) {
    char const *digits="87654321";
    sprintf(HLine, HorizL, digits[row], digits[row]);

    for (column=0, square= square_a;
         column<nr_files_on_board;
         column++, square += dir_right)
    {
      char g = (GridNum(square))%100;
      HLine[fileWidth*column+3]= g>9 ? (g/10)+'0' : ' ';
      HLine[fileWidth*column+4]= (g%10)+'0';
    }

    fputs(HLine,file);
    fputs(BlankL,file);
  }

  fputs(BorderL,file);
}

static void CollectPiecesWithAttribute(position const *pos,
                                       char ListSpec[256],
                                       piece_flag_type sp)
{
  square square_a = square_a8;
  unsigned int row;

  strcpy(ListSpec,PieSpString[UserLanguage][sp-nr_sides]);

  for (row = 1; row<=nr_rows_on_board; ++row, square_a += dir_down)
  {
    unsigned int column;
    square square = square_a;

    for (column = 1; column <= nr_files_on_board; ++column, square += dir_right)
      if (TSTFLAG(pos->spec[square],sp))
        AppendSquare(ListSpec,square);
  }
}

static void WriteNonRoyalAttributedPieces(FILE *file, position const *pos)
{
  piece_flag_type sp;

  for (sp = Royal+1; sp<nr_piece_flags; ++sp)
    if (TSTFLAG(some_pieces_flags,sp))
    {
      if (!(sp==Patrol && CondFlag[patrouille])
          && !(sp==Volage && CondFlag[volage])
          && !(sp==Beamtet && CondFlag[beamten]))
      {
        char ListSpec[256];
        CollectPiecesWithAttribute(pos,ListSpec,sp);
        CenterLine(file,"%s",ListSpec);
      }
    }
}

static unsigned int CollectRoyalPiecePositions(position const *pos,
                                               char ListSpec[256])
{
  unsigned int result = 0;

  square square_a = square_a8;
  unsigned int row;

  strcpy(ListSpec,PieSpString[UserLanguage][Royal-nr_sides]);

  for (row = 0; row!=nr_rows_on_board; ++row, square_a += dir_down)
  {
    unsigned int column;
    square square = square_a;

    for (column = 0; column!=nr_files_on_board; ++column, square += dir_right)
      if (TSTFLAG(pos->spec[square],Royal)
          && !is_king(pos->board[square]))
      {
        AppendSquare(ListSpec,square);
        ++result;
      }
  }

  return result;
}

static void WriteRoyalPiecePositions(FILE *file, position const *pos)
{
  char ListSpec[256];

  if (CollectRoyalPiecePositions(pos,ListSpec)>0)
    CenterLine(file,"%s",ListSpec);
}

static void DoPieceCounts(position const *pos,
                          unsigned piece_per_colour[nr_colours])
{
  square square_a = square_a8;
  unsigned int row;

  for (row = 0; row!=nr_rows_on_board; ++row, square_a += dir_down)
  {
    unsigned int column;
    square square = square_a;

    for (column = 0; column!=nr_files_on_board; ++column, square += dir_right)
    {
      if (is_piece_neutral(pos->spec[square]))
        ++piece_per_colour[colour_neutral];
      else if (TSTFLAG(pos->spec[square],Black))
        ++piece_per_colour[colour_black];
      else if (TSTFLAG(pos->spec[square],White))
        ++piece_per_colour[colour_white];
    }
  }
}

static void WritePieceCounts(FILE *file, position const *pos, unsigned int indentation)
{
  unsigned piece_per_colour[nr_colours] = { 0 };

  DoPieceCounts(pos,piece_per_colour);

  if (piece_per_colour[colour_neutral]>0)
  {
    char dummy;
    int const nr_chars = snprintf(&dummy,1,"%d + %d + %dn",
                                  piece_per_colour[colour_white],
                                  piece_per_colour[colour_black],
                                  piece_per_colour[colour_neutral]);
    int const gap = (nr_files_on_board*fileWidth+3>indentation+nr_chars
                     ? nr_files_on_board*fileWidth-indentation-nr_chars+3
                     : 1);
    fprintf(file,"%*s%d + %d + %dn",
            gap," ",
            piece_per_colour[colour_white],
            piece_per_colour[colour_black],
            piece_per_colour[colour_neutral]);
  }
  else
  {
    char dummy;
    int const nr_chars = snprintf(&dummy,1,"%d + %d",
                                  piece_per_colour[colour_white],
                                  piece_per_colour[colour_black]);
    int const gap = (nr_files_on_board*fileWidth+3>indentation+nr_chars
                     ? nr_files_on_board*fileWidth-indentation-nr_chars+3
                     : 1);
    fprintf(file,"%*s%d + %d",
            gap," ",
            piece_per_colour[colour_white],
            piece_per_colour[colour_black]);
  }
}

static int WriteStipulation(FILE *file)
{
  return fprintf(file,"  %s",AlphaStip);
}

static int WriteOptions(FILE *file, position const *pos)
{
  int result = 0;

  if (OptFlag[solmenaces])
  {
    result += fprintf(file, "/%u", get_max_threat_length());
    if (OptFlag[solflights])
      result += fprintf(file, "/%d", get_max_flights());
  }
  else if (OptFlag[solflights])
    result += fprintf(file, "//%d", get_max_flights());

  if (OptFlag[nontrivial])
    result += fprintf(file,";%d,%u",
                      max_nr_nontrivial,
                      get_min_length_nontrivial());

  return result;
}

static char *WriteWalkRtoL(char *pos, piece_walk_type walk)
{
  pos[0] = PieceTab[walk][1];
  if (pos[0]!=' ')
  {
    pos[0] = toupper(pos[0]);
    --pos;
  }

  pos[0] = toupper(PieceTab[walk][0]);
  --pos;

  return pos;
}

static void WriteRegularCells(FILE *file, position const *pos, square square_a)
{
  unsigned int column;
  square square;

  for (column = 0,  square = square_a;
      column!=nr_files_on_board;
       ++column, square += dir_right)
  {
    char cell[fileWidth+1];
    char *pos_in_cell = cell + (sizeof cell)/2;

    snprintf(cell, sizeof cell, "%*c", fileWidth, ' ');

    if (CondFlag[gridchess] && !OptFlag[suppressgrid])
    {
      if (is_on_board(square+dir_left)
          && GridLegal(square, square+dir_left))
        cell[0] = '|';
    }

    if (is_square_occupied_by_imitator(pos,square))
      pos_in_cell[0] = 'I';
    else if (pos->board[square]==Invalid)
      pos_in_cell[0] = ' ';
    else if (pos->board[square]==Empty)
      pos_in_cell[0] = '.';
    else
    {
      piece_walk_type const walk = pos->board[square];
      if (walk<Hunter0 || walk>=Hunter0+max_nr_hunter_walks)
        pos_in_cell = WriteWalkRtoL(pos_in_cell,walk);
      else
      {
        unsigned int const hunterIndex = walk-Hunter0;
        assert(hunterIndex<max_nr_hunter_walks);

        pos_in_cell[1] = '/';
        pos_in_cell = WriteWalkRtoL(pos_in_cell,huntertypes[hunterIndex].away);
      }

      if (is_piece_neutral(pos->spec[square]))
        pos_in_cell[0] = '=';
      else if (TSTFLAG(pos->spec[square],Black))
        pos_in_cell[0] = '-';
    }

    fputs(cell,file);
  }
}

static void WriteBaseCells(FILE *file, position const *pos, square square_a)
{
  unsigned int column;
  square square;

  for (column = 0, square = square_a;
      column!=nr_files_on_board;
       ++column, square += dir_right)
  {
    piece_walk_type const walk = pos->board[square];

    char cell[fileWidth+1];
    char *pos_in_cell = cell + (sizeof cell)/2;

    snprintf(cell, sizeof cell, "%*c", fileWidth, ' ');

    if (CondFlag[gridchess] && !OptFlag[suppressgrid])
    {
      if (is_on_board(square+dir_down)
          && GridLegal(square,square+dir_down))
      {
        pos_in_cell[-1] = '-';
        pos_in_cell[0] = '-';
        pos_in_cell[+1] = '-';
      }
    }

    if (Hunter0<=walk && walk<Hunter0+max_nr_hunter_walks)
    {
      unsigned int const hunterIndex = walk-Hunter0;
      WriteWalkRtoL(pos_in_cell,huntertypes[hunterIndex].home);
    }

    fputs(cell,file);
  }
}

static void WriteBorder(FILE *file)
{
  unsigned int column;
  char letter;

  assert(nr_files_on_board <= 'z'-'a');

  fputs("+--",file);

  for (column = 0, letter = 'a'; column!=nr_files_on_board; ++column, ++letter)
  {
    char cell[fileWidth+1];
    snprintf(cell, sizeof cell, "-%c--", letter);
    fputs(cell,file);
  }

  fputs("-+\n",file);
}

static void WriteBlankLine(FILE *file)
{
  unsigned int column;

  fputs("| ",file);
  fputs(" ",file);

  for (column = 0; column!=nr_files_on_board; ++column)
    fputs("    ",file);

  fputs(" |\n",file);
}

void WriteBoard(FILE *file, position const *pos)
{
  unsigned int row;
  square square_a;

  assert(nr_rows_on_board<10);

  fputc('\n',file);
  WriteBorder(file);
  WriteBlankLine(file);

  for (row = 0, square_a = square_a8;
       row!=nr_rows_on_board;
       ++row, square_a += dir_down)
  {
    fprintf(file,"%d ",nr_rows_on_board-row);
    WriteRegularCells(file,pos,square_a);
    fprintf(file,"  %d", nr_rows_on_board-row);
    fputc('\n',file);

    fputs("| ",file);
    WriteBaseCells(file,pos,square_a);
    fputs("  |\n",file);
  }

  WriteBorder(file);
}

static void WriteMeta(FILE *file)
{
  fputs("\n",file);
  MultiCenter(file,ActAuthor);
  MultiCenter(file,ActOrigin);
  MultiCenter(file,ActAward);
  MultiCenter(file,ActTitle);
}

static void WriteCondition(FILE* file, char const CondLine[], condition_rank rank)
{
  if (rank!=condition_end)
    CenterLine(file,"%s",CondLine);
}

static void WriteCaptions(FILE *file, position const *pos)
{
  WritePieceCounts(file,pos,WriteStipulation(file)+WriteOptions(file,pos));
  fputc('\n',file);

  WriteRoyalPiecePositions(file,pos);
  WriteNonRoyalAttributedPieces(file,pos);
  WriteConditions(file,&WriteCondition);
  WriteCastlingMutuallyExclusive(file);

  if (OptFlag[halfduplex])
    CenterLine(file,"%s",OptString[UserLanguage][halfduplex]);
  else if (OptFlag[duplex])
    CenterLine(file,"%s",OptString[UserLanguage][duplex]);

  if (OptFlag[quodlibet])
    CenterLine(file,"%s",OptString[UserLanguage][quodlibet]);

  if (CondFlag[gridchess] && OptFlag[writegrid])
    WriteGrid(file);
}

void WritePositionAtoB(FILE *file, Side starter)
{
  WriteMeta(file);
  WriteBoard(file,&proofgames_start_position);
  WritePieceCounts(file,&proofgames_start_position,0);
  fputc('\n',file);

  fputc('\n',file);
  CenterLine(file,"=> (%s ->)",ColourString[UserLanguage][starter]);
  fputc('\n',file);

  WriteBoard(file,&proofgames_target_position);
  WriteCaptions(file,&proofgames_target_position);
}

void WritePositionProofGame(FILE *file)
{
  WriteMeta(file);
  WriteBoard(file,&proofgames_target_position);
  WriteCaptions(file,&proofgames_target_position);
}

void WritePositionRegular(FILE *file)
{
  WriteMeta(file);
  WriteBoard(file,&being_solved);
  WriteCaptions(file,&being_solved);
}
