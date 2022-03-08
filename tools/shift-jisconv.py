#!/usr/bin/env python3

# Converts a file with UTF-8 Japanese glyphs in char literals,
#  into one that uses u16 constants

# Follows the layout in the manual page for easier cross reference
sjis_table = {
	"ー": 0x815B, "ぜ": 0x82BA, "へ": 0x82D6,
	"ぁ": 0x829F, "そ": 0x82BB, "べ": 0x82D7,
	"あ": 0x82A0, "ぞ": 0x82BC, "ぺ": 0x82D8,
	"ぃ": 0x82A1, "た": 0x82BD, "ほ": 0x82D9,
	"い": 0x82A2, "だ": 0x82BE, "ぼ": 0x82DA,
	"ぅ": 0x82A3, "ち": 0x82BF, "ぽ": 0x82DB,
	"う": 0x82A4, "ぢ": 0x82C0,
	"ぇ": 0x82A5, "っ": 0x82C1,
	"え": 0x82A6, "つ": 0x82C2,
	"ぉ": 0x82A7, "づ": 0x82C3,
	"お": 0x82A8, "て": 0x82C4,
	"か": 0x82A9, "で": 0x82C5,
	"が": 0x82AA, "と": 0x82C6,
	"き": 0x82AB, "ど": 0x82C7,
	"ぎ": 0x82AC, "な": 0x82C8,
	"く": 0x82AD, "に": 0x82C9,
	"ぐ": 0x82AE, "ぬ": 0x82CA,
	"け": 0x82AF, "ね": 0x82CB,
	"げ": 0x82B0, "の": 0x82CC,
	"こ": 0x82B1, "は": 0x82CD,
	"ご": 0x82B2, "ば": 0x82CE,
	"さ": 0x82B3, "ぱ": 0x82CF,
	"ざ": 0x82B4, "ひ": 0x82D0,
	"し": 0x82B5, "び": 0x82D1,
	"じ": 0x82B6, "ぴ": 0x82D2,
	"す": 0x82B7, "ふ": 0x82D3,
	"ず": 0x82B8, "ぶ": 0x82D4,
	"せ": 0x82B9, "ぷ": 0x82D5,

	"ま": 0x82DC,
	"み": 0x82DD,
	"む": 0x82DE,
	"め": 0x82DF,
	"も": 0x82E0,

	# Y
	"や": 0x82E2,
	"ゆ": 0x82E4,
	"よ": 0x82E6,

	# R
	"ら": 0x82E7,
	"り": 0x82E8,
	"る": 0x82E9,
	"れ": 0x82EA,
	"ろ": 0x82EB,

	# W
	"わ": 0x82ED,
	"ゐ": 0x82EE,
	"ゑ": 0x82EF,
	"を": 0x82F0,

	# N
	"ん": 0x82F1,
	
	# W (small)
	"ゎ": 0x82EC,

	# A
	"ア": 0x8341,
	"イ": 0x8343,
	"ウ": 0x8345,
	"エ": 0x8347,
	"オ": 0x8349,

	# N
	"ナ": 0x0000,
	"ニ": 0x0000,
	"ヌ": 0x0000,
	"ネ": 0x0000,
	"ノ": 0x0000,

	# M
	"マ": 0x0000,
	"ミ": 0x0000,
	"ム": 0x0000,
	"メ": 0x0000,
	"モ": 0x0000,

	# Y
	"ヤ": 0x0000,
	"ユ": 0x0000,
	"ヨ": 0x0000,

	# R
	"ラ": 0x0000,
	"リ": 0x0000,
	"ル": 0x0000,
	"レ": 0x0000,
	"ロ": 0x0000,

	# W
	"ワ": 0x83BF,
	"ヰ": 0x8390,
	"ヱ": 0x8391,
	"ヲ": 0x8392,

	# N
	"ン": 0x8393,



# Hiragana


	# Y (small)
	"ゃ": 0x82E1,
	"ゅ": 0x82E3,
	"ょ": 0x82E5,

	# K
	"カ": 0x834A,
	"キ": 0x834C,
	"ク": 0x834E,
	"ケ": 0x8350,
	"コ": 0x8352,

	# S
	"サ": 0x8354,
	"シ": 0x8356,
	"ス": 0x8358,
	"セ": 0x835A,
	"ソ": 0x835C,

	# T
	"タ": 0x835E,
	"チ": 0x8360,
	"ツ": 0x8362,
	"テ": 0x8364,
	"ト": 0x8366,

	# H
	"ハ": 0x836E,
	"ヒ": 0x8371,
	"フ": 0x8374,
	"ヘ": 0x8377,
	"ホ": 0x837A,

	# G
	"ガ": 0x0000,
	"ギ": 0x0000,
	"グ": 0x0000,
	"ゲ": 0x0000,
	"ゴ": 0x0000,

	# Z
	"ザ": 0x0000,
	"ジ": 0x0000,
	"ズ": 0x0000,
	"ゼ": 0x0000,
	"ゾ": 0x0000,

	# D
	"ダ": 0x0000,
	"ヂ": 0x0000,
	"ヅ": 0x0000,
	"デ": 0x0000,
	"ド": 0x0000,

	# B
	"バ": 0x836F,
	"ビ": 0x8372,
	"ブ": 0x8375,
	"ベ": 0x8378,
	"ボ": 0x837B,

	# P
	"パ": 0x8370,
	"ピ": 0x8373,
	"プ": 0x8376,
	"ペ": 0x8379,
	"ポ": 0x837C,

	# A (small)
	"ァ": 0x8340,
	"ィ": 0x8342,
	"ゥ": 0x8344,
	"ェ": 0x8346,
	"ォ": 0x8348,

	# Y (small)
	"ャ": 0x83B3,
	"ュ": 0x83B5,
	"ョ": 0x83B7,

	"ッ": 0x8362,
	"ヮ": 0x83BE,

	

}

import sys, os

fb = []

if len(sys.argv) == 1:
	print("Usage: shift-jisconv.py in_file out_file")

with open(sys.argv[1]) as f:
	fb = f.read()

skipTimer = 0


with open(sys.argv[2], "w+") as f:
	for i, char in enumerate(fb):
		if skipTimer > 0:
			skipTimer -= 1
			continue
		if char == "'" and fb[i+1] in sjis_table:
			if sjis_table[fb[i+1]] == 0:
				print("Error: Please map %s in tools/shift-jisconv.py" % fb[i+1])
				exit(1)
			f.write("0x%X" % sjis_table[fb[i+1]])
			skipTimer = 2
		else:
			f.write(char)

