#include "../keyboard.h"

unsigned char klookup[] = {
	KEY_NONE            ,   /* SDLK_FIRST           = 0 */
	KEY_ESC            ,   /* SDLK_NONE            = 1 */
	KEY_NONE            ,   /* SDLK_NONE            = 2 */
	KEY_NONE            ,   /* SDLK_NONE            = 3 */
	KEY_NONE            ,   /* SDLK_NONE            = 4 */
	KEY_NONE            ,   /* SDLK_NONE            = 5 */
	KEY_NONE            ,   /* SDLK_NONE            = 6 */
	KEY_NONE            ,   /* SDLK_NONE            = 7 */
	KEY_BACKSPACE       ,   /* SDLK_BACKSPACE       = 8 */
	KEY_TAB             ,   /* SDLK_TAB             = 9 */
	KEY_NONE            ,   /* SDLK_NONE            = 10 */
	KEY_NONE            ,   /* SDLK_NONE            = 11 */
	KEY_NONE            ,   /* SDLK_CLEAR           = 12 */
	KEY_ENTER           ,   /* SDLK_RETURN          = 13 */
	KEY_NONE            ,   /* SDLK_NONE            = 14 */
	KEY_NONE            ,   /* SDLK_NONE            = 15 */
	KEY_NONE            ,   /* SDLK_NONE            = 16 */
	KEY_NONE            ,   /* SDLK_NONE            = 17 */
	KEY_NONE            ,   /* SDLK_NONE            = 18 */
	KEY_PAUSE           ,   /* SDLK_PAUSE           = 19 */
	KEY_NONE            ,   /* SDLK_NONE            = 20 */
	KEY_NONE            ,   /* SDLK_NONE            = 21 */
	KEY_NONE            ,   /* SDLK_NONE            = 22 */
	KEY_NONE            ,   /* SDLK_NONE            = 23 */
	KEY_NONE            ,   /* SDLK_NONE            = 24 */
	KEY_NONE            ,   /* SDLK_NONE            = 25 */
	KEY_NONE            ,   /* SDLK_NONE            = 26 */
	KEY_ESC             ,   /* SDLK_ESCAPE          = 27 */
	KEY_NONE            ,   /* SDLK_NONE            = 28 */
	KEY_TILDE            ,   /* SDLK_NONE            = 29 */
	KEY_NONE            ,   /* SDLK_NONE            = 30 */
	KEY_NONE            ,   /* SDLK_NONE            = 31 */
	KEY_SPACE           ,   /* SDLK_SPACE           = 32 */
	KEY_NONE            ,   /* SDLK_EXCLAIM         = 33 */
	KEY_QUOTE           ,   /* SDLK_QUOTEDBL        = 34 */
	KEY_NONE            ,   /* SDLK_HASH            = 35 */
	KEY_NONE            ,   /* SDLK_DOLLAR          = 36 */
	KEY_NONE            ,   /* SDLK_NONE            = 37 */
	KEY_NONE            ,   /* SDLK_AMPERSAND       = 38 */
	KEY_QUOTE           ,   /* SDLK_QUOTE           = 39 */
	KEY_OPENBRACE       ,   /* SDLK_LEFTPAREN       = 40 NS */
	KEY_CLOSEBRACE      ,   /* SDLK_RIGHTPAREN      = 41 NS */
	KEY_ASTERISK        ,   /* SDLK_ASTERISK        = 42 */
	KEY_NONE            ,   /* SDLK_PLUS            = 43 */
	KEY_COMMA           ,   /* SDLK_COMMA           = 44 */
	KEY_MINUS           ,   /* SDLK_MINUS           = 45 */
	KEY_STOP            ,   /* SDLK_PERIOD          = 46 */
	KEY_SLASH           ,   /* SDLK_SLASH           = 47 */
	KEY_0               ,   /* SDLK_0               = 48 */
	KEY_1               ,   /* SDLK_1               = 49 */
	KEY_2               ,   /* SDLK_2               = 50 */
	KEY_3               ,   /* SDLK_3               = 51 */
	KEY_4               ,   /* SDLK_4               = 52 */
	KEY_5               ,   /* SDLK_5               = 53 */
	KEY_6               ,   /* SDLK_6               = 54 */
	KEY_7               ,   /* SDLK_7               = 55 */
	KEY_8               ,   /* SDLK_8               = 56 */
	KEY_9               ,   /* SDLK_9               = 57 */
	KEY_F1           ,   /* SDLK_COLON           = 58 */
	KEY_F2           ,   /* SDLK_SEMICOLON       = 59 */
	KEY_F3      ,   /* SDLK_LESS            = 60 */
	KEY_F4          ,   /* SDLK_EQUALS          = 61 */
	KEY_F5            ,   /* SDLK_GREATER         = 62 */
	KEY_F6            ,   /* SDLK_QUESTION        = 63 */
	KEY_F7            ,   /* SDLK_AT              = 64 */
	KEY_F8            ,   /* SDLK_NONE            = 65 */
	KEY_F9            ,   /* SDLK_NONE            = 66 */
	KEY_F10            ,   /* SDLK_NONE            = 67 */
	KEY_F11            ,   /* SDLK_NONE            = 68 */
	KEY_F12            ,   /* SDLK_NONE            = 69 */
	KEY_NONE            ,   /* SDLK_NONE            = 70 */
	KEY_NONE            ,   /* SDLK_NONE            = 71 */
	KEY_NONE            ,   /* SDLK_NONE            = 72 */
	KEY_NONE            ,   /* SDLK_NONE            = 73 */
	KEY_NONE            ,   /* SDLK_NONE            = 74 */
	KEY_NONE            ,   /* SDLK_NONE            = 75 */
	KEY_NONE            ,   /* SDLK_NONE            = 76 */
	KEY_NONE            ,   /* SDLK_NONE            = 77 */
	KEY_NONE            ,   /* SDLK_NONE            = 78 */
	KEY_NONE            ,   /* SDLK_NONE            = 79 */
	KEY_NONE            ,   /* SDLK_NONE            = 80 */
	KEY_NONE            ,   /* SDLK_NONE            = 81 */
	KEY_NONE            ,   /* SDLK_NONE            = 82 */
	KEY_NONE            ,   /* SDLK_NONE            = 83 */
	KEY_NONE            ,   /* SDLK_NONE            = 84 */
	KEY_NONE            ,   /* SDLK_NONE            = 85 */
	KEY_NONE            ,   /* SDLK_NONE            = 86 */
	KEY_NONE            ,   /* SDLK_NONE            = 87 */
	KEY_NONE            ,   /* SDLK_NONE            = 88 */
	KEY_NONE            ,   /* SDLK_NONE            = 89 */
	KEY_NONE            ,   /* SDLK_NONE            = 90 */
	KEY_OPENBRACE       ,   /* SDLK_LEFTBRACKET     = 91 */
	KEY_BACKSLASH       ,   /* SDLK_BACKSLASH       = 92 */
	KEY_CLOSEBRACE      ,   /* SDLK_RIGHTBRACKET    = 93 */
	KEY_NONE            ,   /* SDLK_CARET           = 94 */
	KEY_NONE            ,   /* SDLK_UNDERSCORE      = 95 */
	KEY_TILDE           ,   /* SDLK_BACKQUOTE       = 96 */
	KEY_A               ,   /* SDLK_a               = 97 */
	KEY_B               ,   /* SDLK_b               = 98 */
	KEY_C               ,   /* SDLK_c               = 99 */
	KEY_D               ,   /* SDLK_d               = 100 */
	KEY_E               ,   /* SDLK_e               = 101 */
	KEY_F               ,   /* SDLK_f               = 102 */
	KEY_G               ,   /* SDLK_g               = 103 */
	KEY_H               ,   /* SDLK_h               = 104 */
	KEY_I               ,   /* SDLK_i               = 105 */
	KEY_J               ,   /* SDLK_j               = 106 */
	KEY_K               ,   /* SDLK_k               = 107 */
	KEY_L               ,   /* SDLK_l               = 108 */
	KEY_M               ,   /* SDLK_m               = 109 */
	KEY_N               ,   /* SDLK_n               = 110 */
	KEY_O               ,   /* SDLK_o               = 111 */
	KEY_P               ,   /* SDLK_p               = 112 */
	KEY_Q               ,   /* SDLK_q               = 113 */
	KEY_R               ,   /* SDLK_r               = 114 */
	KEY_S               ,   /* SDLK_s               = 115 */
	KEY_T               ,   /* SDLK_t               = 116 */
	KEY_U               ,   /* SDLK_u               = 117 */
	KEY_V               ,   /* SDLK_v               = 118 */
	KEY_W               ,   /* SDLK_w               = 119 */
	KEY_X               ,   /* SDLK_x               = 120 */
	KEY_Y               ,   /* SDLK_y               = 121 */
	KEY_Z               ,   /* SDLK_z               = 122 */
	KEY_NONE            ,   /* SDLK_NONE            = 123 */
	KEY_NONE            ,   /* SDLK_NONE            = 124 */
	KEY_NONE            ,   /* SDLK_NONE            = 125 */
	KEY_NONE            ,   /* SDLK_NONE            = 126 */
	KEY_DEL             ,   /* SDLK_DELETE          = 127 */
	KEY_NONE            ,   /* SDLK_NONE            = 128 */
	KEY_NONE            ,   /* SDLK_NONE            = 129 */
	KEY_NONE            ,   /* SDLK_NONE            = 130 */
	KEY_NONE            ,   /* SDLK_NONE            = 131 */
	KEY_NONE            ,   /* SDLK_NONE            = 132 */
	KEY_NONE            ,   /* SDLK_NONE            = 133 */
	KEY_NONE            ,   /* SDLK_NONE            = 134 */
	KEY_NONE            ,   /* SDLK_NONE            = 135 */
	KEY_NONE            ,   /* SDLK_NONE            = 136 */
	KEY_NONE            ,   /* SDLK_NONE            = 137 */
	KEY_NONE            ,   /* SDLK_NONE            = 138 */
	KEY_NONE            ,   /* SDLK_NONE            = 139 */
	KEY_NONE            ,   /* SDLK_NONE            = 140 */
	KEY_NONE            ,   /* SDLK_NONE            = 141 */
	KEY_NONE            ,   /* SDLK_NONE            = 142 */
	KEY_NONE            ,   /* SDLK_NONE            = 143 */
	KEY_NONE            ,   /* SDLK_NONE            = 144 */
	KEY_NONE            ,   /* SDLK_NONE            = 145 */
	KEY_NONE            ,   /* SDLK_NONE            = 146 */
	KEY_NONE            ,   /* SDLK_NONE            = 147 */
	KEY_NONE            ,   /* SDLK_NONE            = 148 */
	KEY_NONE            ,   /* SDLK_NONE            = 149 */
	KEY_NONE            ,   /* SDLK_NONE            = 150 */
	KEY_NONE            ,   /* SDLK_NONE            = 151 */
	KEY_NONE            ,   /* SDLK_NONE            = 152 */
	KEY_NONE            ,   /* SDLK_NONE            = 153 */
	KEY_NONE            ,   /* SDLK_NONE            = 154 */
	KEY_NONE            ,   /* SDLK_NONE            = 155 */
	KEY_NONE            ,   /* SDLK_NONE            = 156 */
	KEY_NONE            ,   /* SDLK_NONE            = 157 */
	KEY_NONE            ,   /* SDLK_NONE            = 158 */
	KEY_NONE            ,   /* SDLK_NONE            = 159 */
	KEY_NONE            ,   /* SDLK_WORLD_0         = 160 */
	KEY_NONE            ,   /* SDLK_WORLD_1         = 161 */
	KEY_NONE            ,   /* SDLK_WORLD_2         = 162 */
	KEY_NONE            ,   /* SDLK_WORLD_3         = 163 */
	KEY_NONE            ,   /* SDLK_WORLD_4         = 164 */
	KEY_NONE            ,   /* SDLK_WORLD_5         = 165 */
	KEY_NONE            ,   /* SDLK_WORLD_6         = 166 */
	KEY_NONE            ,   /* SDLK_WORLD_7         = 167 */
	KEY_NONE            ,   /* SDLK_WORLD_8         = 168 */
	KEY_NONE            ,   /* SDLK_WORLD_9         = 169 */
	KEY_NONE            ,   /* SDLK_WORLD_10        = 170 */
	KEY_NONE            ,   /* SDLK_WORLD_11        = 171 */
	KEY_NONE            ,   /* SDLK_WORLD_12        = 172 */
	KEY_NONE            ,   /* SDLK_WORLD_13        = 173 */
	KEY_NONE            ,   /* SDLK_WORLD_14        = 174 */
	KEY_NONE            ,   /* SDLK_WORLD_15        = 175 */
	KEY_NONE            ,   /* SDLK_WORLD_16        = 176 */
	KEY_NONE            ,   /* SDLK_WORLD_17        = 177 */
	KEY_NONE            ,   /* SDLK_WORLD_18        = 178 */
	KEY_NONE            ,   /* SDLK_WORLD_19        = 179 */
	KEY_NONE            ,   /* SDLK_WORLD_20        = 180 */
	KEY_NONE            ,   /* SDLK_WORLD_21        = 181 */
	KEY_NONE            ,   /* SDLK_WORLD_22        = 182 */
	KEY_NONE            ,   /* SDLK_WORLD_23        = 183 */
	KEY_NONE            ,   /* SDLK_WORLD_24        = 184 */
	KEY_NONE            ,   /* SDLK_WORLD_25        = 185 */
	KEY_NONE            ,   /* SDLK_WORLD_26        = 186 */
	KEY_NONE            ,   /* SDLK_WORLD_27        = 187 */
	KEY_NONE            ,   /* SDLK_WORLD_28        = 188 */
	KEY_NONE            ,   /* SDLK_WORLD_29        = 189 */
	KEY_NONE            ,   /* SDLK_WORLD_30        = 190 */
	KEY_NONE            ,   /* SDLK_WORLD_31        = 191 */
	KEY_NONE            ,   /* SDLK_WORLD_32        = 192 */
	KEY_NONE            ,   /* SDLK_WORLD_33        = 193 */
	KEY_NONE            ,   /* SDLK_WORLD_34        = 194 */
	KEY_NONE            ,   /* SDLK_WORLD_35        = 195 */
	KEY_NONE            ,   /* SDLK_WORLD_36        = 196 */
	KEY_NONE            ,   /* SDLK_WORLD_37        = 197 */
	KEY_NONE            ,   /* SDLK_WORLD_38        = 198 */
	KEY_NONE            ,   /* SDLK_WORLD_39        = 199 */
	KEY_NONE            ,   /* SDLK_WORLD_40        = 200 */
	KEY_NONE            ,   /* SDLK_WORLD_41        = 201 */
	KEY_NONE            ,   /* SDLK_WORLD_42        = 202 */
	KEY_NONE            ,   /* SDLK_WORLD_43        = 203 */
	KEY_NONE            ,   /* SDLK_WORLD_44        = 204 */
	KEY_NONE            ,   /* SDLK_WORLD_45        = 205 */
	KEY_NONE            ,   /* SDLK_WORLD_46        = 206 */
	KEY_NONE            ,   /* SDLK_WORLD_47        = 207 */
	KEY_NONE            ,   /* SDLK_WORLD_48        = 208 */
	KEY_NONE            ,   /* SDLK_WORLD_49        = 209 */
	KEY_NONE            ,   /* SDLK_WORLD_50        = 210 */
	KEY_NONE            ,   /* SDLK_WORLD_51        = 211 */
	KEY_NONE            ,   /* SDLK_WORLD_52        = 212 */
	KEY_NONE            ,   /* SDLK_WORLD_53        = 213 */
	KEY_NONE            ,   /* SDLK_WORLD_54        = 214 */
	KEY_NONE            ,   /* SDLK_WORLD_55        = 215 */
	KEY_NONE            ,   /* SDLK_WORLD_56        = 216 */
	KEY_NONE            ,   /* SDLK_WORLD_57        = 217 */
	KEY_NONE            ,   /* SDLK_WORLD_58        = 218 */
	KEY_NONE            ,   /* SDLK_WORLD_59        = 219 */
	KEY_NONE            ,   /* SDLK_WORLD_60        = 220 */
	KEY_NONE            ,   /* SDLK_WORLD_61        = 221 */
	KEY_NONE            ,   /* SDLK_WORLD_62        = 222 */
	KEY_NONE            ,   /* SDLK_WORLD_63        = 223 */
	KEY_NONE            ,   /* SDLK_WORLD_64        = 224 */
	KEY_NONE            ,   /* SDLK_WORLD_65        = 225 */
	KEY_NONE            ,   /* SDLK_WORLD_66        = 226 */
	KEY_NONE            ,   /* SDLK_WORLD_67        = 227 */
	KEY_NONE            ,   /* SDLK_WORLD_68        = 228 */
	KEY_NONE            ,   /* SDLK_WORLD_69        = 229 */
	KEY_NONE            ,   /* SDLK_WORLD_70        = 230 */
	KEY_NONE            ,   /* SDLK_WORLD_71        = 231 */
	KEY_NONE            ,   /* SDLK_WORLD_72        = 232 */
	KEY_NONE            ,   /* SDLK_WORLD_73        = 233 */
	KEY_NONE            ,   /* SDLK_WORLD_74        = 234 */
	KEY_NONE            ,   /* SDLK_WORLD_75        = 235 */
	KEY_NONE            ,   /* SDLK_WORLD_76        = 236 */
	KEY_NONE            ,   /* SDLK_WORLD_77        = 237 */
	KEY_NONE            ,   /* SDLK_WORLD_78        = 238 */
	KEY_NONE            ,   /* SDLK_WORLD_79        = 239 */
	KEY_NONE            ,   /* SDLK_WORLD_80        = 240 */
	KEY_NONE            ,   /* SDLK_WORLD_81        = 241 */
	KEY_NONE            ,   /* SDLK_WORLD_82        = 242 */
	KEY_NONE            ,   /* SDLK_WORLD_83        = 243 */
	KEY_NONE            ,   /* SDLK_WORLD_84        = 244 */
	KEY_NONE            ,   /* SDLK_WORLD_85        = 245 */
	KEY_NONE            ,   /* SDLK_WORLD_86        = 246 */
	KEY_NONE            ,   /* SDLK_WORLD_87        = 247 */
	KEY_NONE            ,   /* SDLK_WORLD_88        = 248 */
	KEY_NONE            ,   /* SDLK_WORLD_89        = 249 */
	KEY_NONE            ,   /* SDLK_WORLD_90        = 250 */
	KEY_NONE            ,   /* SDLK_WORLD_91        = 251 */
	KEY_NONE            ,   /* SDLK_WORLD_92        = 252 */
	KEY_NONE            ,   /* SDLK_WORLD_93        = 253 */
	KEY_NONE            ,   /* SDLK_WORLD_94        = 254 */
	KEY_NONE            ,   /* SDLK_WORLD_95        = 255 */
	KEY_0_PAD           ,   /* SDLK_KP0             = 256 */
	KEY_1_PAD           ,   /* SDLK_KP1             = 257 */
	KEY_2_PAD           ,   /* SDLK_KP2             = 258 */
	KEY_3_PAD           ,   /* SDLK_KP3             = 259 */
	KEY_4_PAD           ,   /* SDLK_KP4             = 260 */
	KEY_5_PAD           ,   /* SDLK_KP5             = 261 */
	KEY_6_PAD           ,   /* SDLK_KP6             = 262 */
	KEY_7_PAD           ,   /* SDLK_KP7             = 263 */
	KEY_8_PAD           ,   /* SDLK_KP8             = 264 */
	KEY_9_PAD           ,   /* SDLK_KP9             = 265 */
	KEY_DEL_PAD         ,   /* SDLK_KP_PERIOD       = 266 */
	KEY_SLASH_PAD       ,   /* SDLK_KP_DIVIDE       = 267 NS */
	KEY_ASTERISK        ,   /* SDLK_KP_MULTIPLY     = 268 */
	KEY_MINUS_PAD       ,   /* SDLK_KP_MINUS        = 269 */
	KEY_PLUS_PAD        ,   /* SDLK_KP_PLUS         = 270 */
	KEY_ENTER_PAD       ,   /* SDLK_KP_ENTER        = 271 */
	KEY_EQUALS          ,   /* SDLK_KP_EQUALS       = 272 */
	KEY_UP              ,   /* SDLK_UP              = 273 */
	KEY_DOWN            ,   /* SDLK_DOWN            = 274 */
	KEY_RIGHT           ,   /* SDLK_RIGHT           = 275 */
	KEY_LEFT            ,   /* SDLK_LEFT            = 276 */
	KEY_INSERT          ,   /* SDLK_INSERT          = 277 */
	KEY_HOME            ,   /* SDLK_HOME            = 278 */
	KEY_END             ,   /* SDLK_END             = 279 */
	KEY_PGUP            ,   /* SDLK_PAGEUP          = 280 */
	KEY_PGDN            ,   /* SDLK_PAGEDOWN        = 281 */
	KEY_F1              ,   /* SDLK_F1              = 282 */
	KEY_F2              ,   /* SDLK_F2              = 283 */
	KEY_F3              ,   /* SDLK_F3              = 284 */
	KEY_F4              ,   /* SDLK_F4              = 285 */
	KEY_F5              ,   /* SDLK_F5              = 286 */
	KEY_F6              ,   /* SDLK_F6              = 287 */
	KEY_F7              ,   /* SDLK_F7              = 288 */
	KEY_F8              ,   /* SDLK_F8              = 289 */
	KEY_F9              ,   /* SDLK_F9              = 290 */
	KEY_F10             ,   /* SDLK_F10             = 291 */
	KEY_F11             ,   /* SDLK_F11             = 292 */
	KEY_F12             ,   /* SDLK_F12             = 293 */
	KEY_NONE            ,   /* SDLK_F13             = 294 */
	KEY_NONE            ,   /* SDLK_F14             = 295 */
	KEY_NONE            ,   /* SDLK_F15             = 296 */
	KEY_NONE            ,   /* SDLK_NONE            = 297 */
	KEY_NONE            ,   /* SDLK_NONE            = 298 */
	KEY_NONE            ,   /* SDLK_NONE            = 299 */
	KEY_NUMLOCK         ,   /* SDLK_NUMLOCK         = 300 */
	KEY_CAPSLOCK        ,   /* SDLK_CAPSLOCK        = 301 */
	KEY_SCRLOCK         ,   /* SDLK_SCROLLOCK       = 302 */
	KEY_RSHIFT          ,   /* SDLK_RSHIFT          = 303 */
	KEY_LSHIFT          ,   /* SDLK_LSHIFT          = 304 */
	KEY_RCONTROL        ,   /* SDLK_RCTRL           = 305 */
	KEY_LCONTROL        ,   /* SDLK_LCTRL           = 306 */
	KEY_ALTGR           ,   /* SDLK_RALT            = 307 NS */
	KEY_ALT             ,   /* SDLK_LALT            = 308 NS */
	KEY_ALTGR           ,   /* SDLK_RMETA           = 309 NS */
	KEY_ALT             ,   /* SDLK_LMETA           = 310 NS */
	KEY_NONE            ,   /* SDLK_LSUPER          = 311 */
	KEY_NONE            ,   /* SDLK_RSUPER          = 312 */
	KEY_NONE            ,   /* SDLK_MODE            = 313 */
	KEY_NONE            ,   /* SDLK_NONE            = 314 */
	KEY_NONE            ,   /* SDLK_HELP            = 315 */
	KEY_PRTSCR          ,   /* SDLK_PRINT           = 316 NS */
	KEY_NONE            ,   /* SDLK_SYSREQ          = 317 */
	KEY_NONE            ,   /* SDLK_BREAK           = 318 */
	KEY_MENU            ,   /* SDLK_MENU            = 319 */
	KEY_NONE            ,   /* SDLK_POWER           = 320 */
	KEY_NONE            ,   /* SDLK_EURO            = 321 */
	KEY_NONE            ,   /* SDLK_NONE            = 322 */
	KEY_NONE            ,   /* SDLK_NONE            = 323 */
	KEY_NONE            ,   /* SDLK_NONE            = 324 */
	KEY_NONE            ,   /* SDLK_NONE            = 325 */
	KEY_NONE            ,   /* SDLK_NONE            = 326 */
	KEY_NONE            ,   /* SDLK_NONE            = 327 */
	KEY_NONE            ,   /* SDLK_NONE            = 328 */
	KEY_NONE            ,   /* SDLK_NONE            = 329 */
	KEY_NONE            ,   /* SDLK_NONE            = 330 */
	KEY_NONE            ,   /* SDLK_NONE            = 331 */
	KEY_NONE            ,   /* SDLK_NONE            = 332 */
	KEY_NONE            ,   /* SDLK_NONE            = 333 */
	KEY_NONE            ,   /* SDLK_NONE            = 334 */
	KEY_NONE            ,   /* SDLK_NONE            = 335 */
	KEY_NONE            ,   /* SDLK_NONE            = 336 */
	KEY_NONE            ,   /* SDLK_NONE            = 337 */
	KEY_NONE            ,   /* SDLK_NONE            = 338 */
	KEY_NONE            ,   /* SDLK_NONE            = 339 */
	KEY_NONE            ,   /* SDLK_NONE            = 340 */
	KEY_NONE            ,   /* SDLK_NONE            = 341 */
	KEY_NONE            ,   /* SDLK_NONE            = 342 */
	KEY_NONE            ,   /* SDLK_NONE            = 343 */
	KEY_NONE            ,   /* SDLK_NONE            = 344 */
	KEY_NONE            ,   /* SDLK_NONE            = 345 */
	KEY_NONE            ,   /* SDLK_NONE            = 346 */
	KEY_NONE            ,   /* SDLK_NONE            = 347 */
	KEY_NONE            ,   /* SDLK_NONE            = 348 */
	KEY_NONE            ,   /* SDLK_NONE            = 349 */
	KEY_NONE            ,   /* SDLK_NONE            = 350 */
	KEY_NONE            ,   /* SDLK_NONE            = 351 */
	KEY_NONE            ,   /* SDLK_NONE            = 352 */
	KEY_NONE            ,   /* SDLK_NONE            = 353 */
	KEY_NONE            ,   /* SDLK_NONE            = 354 */
	KEY_NONE            ,   /* SDLK_NONE            = 355 */
	KEY_NONE            ,   /* SDLK_NONE            = 356 */
	KEY_NONE            ,   /* SDLK_NONE            = 357 */
	KEY_NONE            ,   /* SDLK_NONE            = 358 */
	KEY_NONE            ,   /* SDLK_NONE            = 359 */
	KEY_NONE            ,   /* SDLK_NONE            = 360 */
	KEY_NONE            ,   /* SDLK_NONE            = 361 */
	KEY_NONE            ,   /* SDLK_NONE            = 362 */
	KEY_NONE            ,   /* SDLK_NONE            = 363 */
	KEY_NONE            ,   /* SDLK_NONE            = 364 */
	KEY_NONE            ,   /* SDLK_NONE            = 365 */
	KEY_NONE            ,   /* SDLK_NONE            = 366 */
	KEY_NONE            ,   /* SDLK_NONE            = 367 */
	KEY_NONE            ,   /* SDLK_NONE            = 368 */
	KEY_NONE            ,   /* SDLK_NONE            = 369 */
	KEY_NONE            ,   /* SDLK_NONE            = 370 */
	KEY_NONE            ,   /* SDLK_NONE            = 371 */
	KEY_NONE            ,   /* SDLK_NONE            = 372 */
	KEY_NONE            ,   /* SDLK_NONE            = 373 */
	KEY_NONE            ,   /* SDLK_NONE            = 374 */
	KEY_NONE            ,   /* SDLK_NONE            = 375 */
	KEY_NONE            ,   /* SDLK_NONE            = 376 */
	KEY_NONE            ,   /* SDLK_NONE            = 377 */
	KEY_NONE            ,   /* SDLK_NONE            = 378 */
	KEY_NONE            ,   /* SDLK_NONE            = 379 */
	KEY_NONE            ,   /* SDLK_NONE            = 380 */
	KEY_NONE            ,   /* SDLK_NONE            = 381 */
	KEY_NONE            ,   /* SDLK_NONE            = 382 */
	KEY_NONE            ,   /* SDLK_NONE            = 383 */
	KEY_NONE            ,   /* SDLK_NONE            = 384 */
	KEY_NONE            ,   /* SDLK_NONE            = 385 */
	KEY_NONE            ,   /* SDLK_NONE            = 386 */
	KEY_NONE            ,   /* SDLK_NONE            = 387 */
	KEY_NONE            ,   /* SDLK_NONE            = 388 */
	KEY_NONE            ,   /* SDLK_NONE            = 389 */
	KEY_NONE            ,   /* SDLK_NONE            = 390 */
	KEY_NONE            ,   /* SDLK_NONE            = 391 */
	KEY_NONE            ,   /* SDLK_NONE            = 392 */
	KEY_NONE            ,   /* SDLK_NONE            = 393 */
	KEY_NONE            ,   /* SDLK_NONE            = 394 */
	KEY_NONE            ,   /* SDLK_NONE            = 395 */
	KEY_NONE            ,   /* SDLK_NONE            = 396 */
	KEY_NONE            ,   /* SDLK_NONE            = 397 */
	KEY_NONE            ,   /* SDLK_NONE            = 398 */
	KEY_NONE            ,   /* SDLK_NONE            = 399 */
	KEY_NONE            ,   /* SDLK_NONE            = 400 */
	KEY_NONE            ,   /* SDLK_NONE            = 401 */
	KEY_NONE            ,   /* SDLK_NONE            = 402 */
	KEY_NONE            ,   /* SDLK_NONE            = 403 */
	KEY_NONE            ,   /* SDLK_NONE            = 404 */
	KEY_NONE            ,   /* SDLK_NONE            = 405 */
	KEY_NONE            ,   /* SDLK_NONE            = 406 */
	KEY_NONE            ,   /* SDLK_NONE            = 407 */
	KEY_NONE            ,   /* SDLK_NONE            = 408 */
	KEY_NONE            ,   /* SDLK_NONE            = 409 */
	KEY_NONE            ,   /* SDLK_NONE            = 410 */
	KEY_NONE            ,   /* SDLK_NONE            = 411 */
	KEY_NONE            ,   /* SDLK_NONE            = 412 */
	KEY_NONE            ,   /* SDLK_NONE            = 413 */
	KEY_NONE            ,   /* SDLK_NONE            = 414 */
	KEY_NONE            ,   /* SDLK_NONE            = 415 */
	KEY_NONE            ,   /* SDLK_NONE            = 416 */
	KEY_NONE            ,   /* SDLK_NONE            = 417 */
	KEY_NONE            ,   /* SDLK_NONE            = 418 */
	KEY_NONE            ,   /* SDLK_NONE            = 419 */
	KEY_NONE            ,   /* SDLK_NONE            = 420 */
	KEY_NONE            ,   /* SDLK_NONE            = 421 */
	KEY_NONE            ,   /* SDLK_NONE            = 422 */
	KEY_NONE            ,   /* SDLK_NONE            = 423 */
	KEY_NONE            ,   /* SDLK_NONE            = 424 */
	KEY_NONE            ,   /* SDLK_NONE            = 425 */
	KEY_NONE            ,   /* SDLK_NONE            = 426 */
	KEY_NONE            ,   /* SDLK_NONE            = 427 */
	KEY_NONE            ,   /* SDLK_NONE            = 428 */
	KEY_NONE            ,   /* SDLK_NONE            = 429 */
	KEY_NONE            ,   /* SDLK_NONE            = 430 */
	KEY_NONE            ,   /* SDLK_NONE            = 431 */
	KEY_NONE            ,   /* SDLK_NONE            = 432 */
	KEY_NONE            ,   /* SDLK_NONE            = 433 */
	KEY_NONE            ,   /* SDLK_NONE            = 434 */
	KEY_NONE            ,   /* SDLK_NONE            = 435 */
	KEY_NONE            ,   /* SDLK_NONE            = 436 */
	KEY_NONE            ,   /* SDLK_NONE            = 437 */
	KEY_NONE            ,   /* SDLK_NONE            = 438 */
	KEY_NONE            ,   /* SDLK_NONE            = 439 */
	KEY_NONE            ,   /* SDLK_NONE            = 440 */
	KEY_NONE            ,   /* SDLK_NONE            = 441 */
	KEY_NONE            ,   /* SDLK_NONE            = 442 */
	KEY_NONE            ,   /* SDLK_NONE            = 443 */
	KEY_NONE            ,   /* SDLK_NONE            = 444 */
	KEY_NONE            ,   /* SDLK_NONE            = 445 */
	KEY_NONE            ,   /* SDLK_NONE            = 446 */
	KEY_NONE            ,   /* SDLK_NONE            = 447 */
	KEY_NONE            ,   /* SDLK_NONE            = 448 */
	KEY_NONE            ,   /* SDLK_NONE            = 449 */
	KEY_NONE            ,   /* SDLK_NONE            = 450 */
	KEY_NONE            ,   /* SDLK_NONE            = 451 */
	KEY_NONE            ,   /* SDLK_NONE            = 452 */
	KEY_NONE            ,   /* SDLK_NONE            = 453 */
	KEY_NONE            ,   /* SDLK_NONE            = 454 */
	KEY_NONE            ,   /* SDLK_NONE            = 455 */
	KEY_NONE            ,   /* SDLK_NONE            = 456 */
	KEY_NONE            ,   /* SDLK_NONE            = 457 */
	KEY_NONE            ,   /* SDLK_NONE            = 458 */
	KEY_NONE            ,   /* SDLK_NONE            = 459 */
	KEY_NONE            ,   /* SDLK_NONE            = 460 */
	KEY_NONE            ,   /* SDLK_NONE            = 461 */
	KEY_NONE            ,   /* SDLK_NONE            = 462 */
	KEY_NONE            ,   /* SDLK_NONE            = 463 */
	KEY_NONE            ,   /* SDLK_NONE            = 464 */
	KEY_NONE            ,   /* SDLK_NONE            = 465 */
	KEY_NONE            ,   /* SDLK_NONE            = 466 */
	KEY_NONE            ,   /* SDLK_NONE            = 467 */
	KEY_NONE            ,   /* SDLK_NONE            = 468 */
	KEY_NONE            ,   /* SDLK_NONE            = 469 */
	KEY_NONE            ,   /* SDLK_NONE            = 470 */
	KEY_NONE            ,   /* SDLK_NONE            = 471 */
	KEY_NONE            ,   /* SDLK_NONE            = 472 */
	KEY_NONE            ,   /* SDLK_NONE            = 473 */
	KEY_NONE            ,   /* SDLK_NONE            = 474 */
	KEY_NONE            ,   /* SDLK_NONE            = 475 */
	KEY_NONE            ,   /* SDLK_NONE            = 476 */
	KEY_NONE            ,   /* SDLK_NONE            = 477 */
	KEY_NONE            ,   /* SDLK_NONE            = 478 */
	KEY_NONE            ,   /* SDLK_NONE            = 479 */
	KEY_NONE            ,   /* SDLK_NONE            = 480 */
	KEY_NONE            ,   /* SDLK_NONE            = 481 */
	KEY_NONE            ,   /* SDLK_NONE            = 482 */
	KEY_NONE            ,   /* SDLK_NONE            = 483 */
	KEY_NONE            ,   /* SDLK_NONE            = 484 */
	KEY_NONE            ,   /* SDLK_NONE            = 485 */
	KEY_NONE            ,   /* SDLK_NONE            = 486 */
	KEY_NONE            ,   /* SDLK_NONE            = 487 */
	KEY_NONE            ,   /* SDLK_NONE            = 488 */
	KEY_NONE            ,   /* SDLK_NONE            = 489 */
	KEY_NONE            ,   /* SDLK_NONE            = 490 */
	KEY_NONE            ,   /* SDLK_NONE            = 491 */
	KEY_NONE            ,   /* SDLK_NONE            = 492 */
	KEY_NONE            ,   /* SDLK_NONE            = 493 */
	KEY_NONE            ,   /* SDLK_NONE            = 494 */
	KEY_NONE            ,   /* SDLK_NONE            = 495 */
	KEY_NONE            ,   /* SDLK_NONE            = 496 */
	KEY_NONE            ,   /* SDLK_NONE            = 497 */
	KEY_NONE            ,   /* SDLK_NONE            = 498 */
	KEY_NONE            ,   /* SDLK_NONE            = 499 */
	KEY_NONE            ,   /* SDLK_NONE            = 500 */
	KEY_NONE            ,   /* SDLK_NONE            = 501 */
	KEY_NONE            ,   /* SDLK_NONE            = 502 */
	KEY_NONE            ,   /* SDLK_NONE            = 503 */
	KEY_NONE            ,   /* SDLK_NONE            = 504 */
	KEY_NONE            ,   /* SDLK_NONE            = 505 */
	KEY_NONE            ,   /* SDLK_NONE            = 506 */
	KEY_NONE            ,   /* SDLK_NONE            = 507 */
	KEY_NONE            ,   /* SDLK_NONE            = 508 */
	KEY_NONE            ,   /* SDLK_NONE            = 509 */
	KEY_NONE            ,   /* SDLK_NONE            = 510 */
	KEY_NONE                /* SDLK_NONE            = 511 */
};




























































































































































































































































unsigned char klookuphigh[] = {
KEY_TILDE			, //            0x40000000	
KEY_NONE				, //            0x40000001	
KEY_NONE				, //            0x40000002	
KEY_NONE				, //            0x40000003	
KEY_NONE				, //            0x40000004	
KEY_NONE				, //            0x40000005	
KEY_NONE				, //            0x40000006	
KEY_NONE				, //            0x40000007	
KEY_NONE				, //            0x40000008	
KEY_NONE				, //            0x40000009	
KEY_NONE				, //            0x4000000a	
KEY_NONE				, //            0x4000000b	
KEY_NONE				, //            0x4000000c	
KEY_NONE				, //            0x4000000d	
KEY_NONE				, //            0x4000000e	
KEY_NONE				, //            0x4000000f	
KEY_NONE				, //            0x40000010	
KEY_NONE				, //            0x40000011	
KEY_NONE				, //            0x40000012	
KEY_NONE				, //            0x40000013	
KEY_NONE				, //            0x40000014	
KEY_NONE				, //            0x40000015	
KEY_NONE				, //            0x40000016	
KEY_NONE				, //            0x40000017	
KEY_NONE				, //            0x40000018	
KEY_NONE				, //            0x40000019	
KEY_NONE				, //            0x4000001a	
KEY_NONE				, //            0x4000001b	
KEY_NONE				, //            0x4000001c	
KEY_NONE				, //            0x4000001d	
KEY_NONE				, //            0x4000001e	
KEY_NONE				, //            0x4000001f	
KEY_NONE				, //            0x40000020	
KEY_NONE				, //            0x40000021	
KEY_NONE				, //            0x40000022	
KEY_NONE				, //            0x40000023	
KEY_NONE				, //            0x40000024	
KEY_NONE				, //            0x40000025	
KEY_NONE				, //            0x40000026	
KEY_NONE				, //            0x40000027	
KEY_NONE				, //            0x40000028	
KEY_NONE				, //            0x40000029	
KEY_NONE				, //            0x4000002a	
KEY_NONE				, //            0x4000002b	
KEY_NONE				, //            0x4000002c	
KEY_NONE				, //            0x4000002d	
KEY_NONE				, //            0x4000002e	
KEY_NONE				, //            0x4000002f	
KEY_NONE				, //            0x40000030	
KEY_NONE				, //            0x40000031	
KEY_NONE				, //            0x40000032	
KEY_NONE				, //            0x40000033	
KEY_NONE				, //            0x40000034	
KEY_NONE				, //            0x40000035	
KEY_NONE				, //            0x40000036	
KEY_NONE				, //            0x40000037	
KEY_NONE				, //            0x40000038	
KEY_CAPSLOCK            , // 1073741881 0x40000039 SDLK_CAPSLOCK
KEY_F1                  , // 1073741882 0x4000003A SDLK_F1
KEY_F2                  , // 1073741883 0x4000003B SDLK_F2
KEY_F3                  , // 1073741884 0x4000003C SDLK_F3
KEY_NONE                , // 1073741885 0x4000003D SDLK_F4
KEY_NONE                , // 1073741886 0x4000003E SDLK_F5
KEY_NONE                , // 1073741887 0x4000003F SDLK_F6
KEY_NONE                , // 1073741888 0x40000040 SDLK_F7
KEY_NONE                , // 1073741889 0x40000041 SDLK_F8
KEY_NONE                , // 1073741890 0x40000042 SDLK_F9
KEY_NONE                , // 1073741891 0x40000043 SDLK_F10
KEY_NONE                , // 1073741892 0x40000044 SDLK_F11
KEY_NONE                , // 1073741893 0x40000045 SDLK_F12
KEY_NONE                , // 1073741894 0x40000046 SDLK_PRINTSCREEN
KEY_NONE                , // 1073741895 0x40000047 SDLK_SCROLLLOCK
KEY_NONE                , // 1073741896 0x40000048 SDLK_PAUSE
KEY_NONE                , // 1073741897 0x40000049 SDLK_INSERT
KEY_NONE                , // 1073741898 0x4000004A SDLK_HOME
KEY_NONE                , // 1073741899 0x4000004B SDLK_PAGEUP
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073741901 0x4000004D SDLK_END
KEY_NONE                , // 1073741902 0x4000004E SDLK_PAGEDOWN
KEY_NONE                , // 1073741903 0x4000004F SDLK_RIGHT
KEY_NONE                , // 1073741904 0x40000050 SDLK_LEFT
KEY_NONE                , // 1073741905 0x40000051 SDLK_DOWN
KEY_NONE                , // 1073741906 0x40000052 SDLK_UP
KEY_NONE                , // 1073741907 0x40000053 SDLK_NUMLOCKCLEAR
KEY_NONE                , // 1073741908 0x40000054 SDLK_KP_DIVIDE
KEY_NONE                , // 1073741909 0x40000055 SDLK_KP_MULTIPLY
KEY_NONE                , // 1073741910 0x40000056 SDLK_KP_MINUS
KEY_NONE                , // 1073741911 0x40000057 SDLK_KP_PLUS
KEY_NONE                , // 1073741912 0x40000058 SDLK_KP_ENTER
KEY_NONE                , // 1073741913 0x40000059 SDLK_KP_1
KEY_NONE                , // 1073741914 0x4000005A SDLK_KP_2
KEY_NONE                , // 1073741915 0x4000005B SDLK_KP_3
KEY_NONE                , // 1073741916 0x4000005C SDLK_KP_4
KEY_NONE                , // 1073741917 0x4000005D SDLK_KP_5
KEY_NONE                , // 1073741918 0x4000005E SDLK_KP_6
KEY_NONE                , // 1073741919 0x4000005F SDLK_KP_7
KEY_NONE                , // 1073741920 0x40000060 SDLK_KP_8
KEY_NONE                , // 1073741921 0x40000061 SDLK_KP_9
KEY_NONE                , // 1073741922 0x40000062 SDLK_KP_0
KEY_NONE                , // 1073741923 0x40000063 SDLK_KP_PERIOD
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073741925 0x40000065 SDLK_APPLICATION
KEY_NONE                , // 1073741926 0x40000066 SDLK_POWER
KEY_NONE                , // 1073741927 0x40000067 SDLK_KP_EQUALS
KEY_NONE                , // 1073741928 0x40000068 SDLK_F13
KEY_NONE                , // 1073741929 0x40000069 SDLK_F14
KEY_NONE                , // 1073741930 0x4000006A SDLK_F15
KEY_NONE                , // 1073741931 0x4000006B SDLK_F16
KEY_NONE                , // 1073741932 0x4000006C SDLK_F17
KEY_NONE                , // 1073741933 0x4000006D SDLK_F18
KEY_NONE                , // 1073741934 0x4000006E SDLK_F19
KEY_NONE                , // 1073741935 0x4000006F SDLK_F20
KEY_NONE                , // 1073741936 0x40000070 SDLK_F21
KEY_NONE                , // 1073741937 0x40000071 SDLK_F22
KEY_NONE                , // 1073741938 0x40000072 SDLK_F23
KEY_NONE                , // 1073741939 0x40000073 SDLK_F24
KEY_NONE                , // 1073741940 0x40000074 SDLK_EXECUTE
KEY_NONE                , // 1073741941 0x40000075 SDLK_HELP
KEY_NONE                , // 1073741942 0x40000076 SDLK_MENU
KEY_NONE                , // 1073741943 0x40000077 SDLK_SELECT
KEY_NONE                , // 1073741944 0x40000078 SDLK_STOP
KEY_NONE                , // 1073741945 0x40000079 SDLK_AGAIN
KEY_NONE                , // 1073741946 0x4000007A SDLK_UNDO
KEY_NONE                , // 1073741947 0x4000007B SDLK_CUT
KEY_NONE                , // 1073741948 0x4000007C SDLK_COPY
KEY_NONE                , // 1073741949 0x4000007D SDLK_PASTE
KEY_NONE                , // 1073741950 0x4000007E SDLK_FIND
KEY_NONE                , // 1073741951 0x4000007F SDLK_MUTE
KEY_NONE                , // 1073741952 0x40000080 SDLK_VOLUMEUP
KEY_NONE                , // 1073741953 0x40000081 SDLK_VOLUMEDOWN
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073741957 0x40000085 SDLK_KP_COMMA
KEY_NONE                , // 1073741958 0x40000086 SDLK_KP_EQUALSAS400
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073741977 0x40000099 SDLK_ALTERASE
KEY_NONE                , // 1073741978 0x4000009A SDLK_SYSREQ
KEY_NONE                , // 1073741979 0x4000009B SDLK_CANCEL
KEY_NONE                , // 1073741980 0x4000009C SDLK_CLEAR
KEY_NONE                , // 1073741981 0x4000009D SDLK_PRIOR
KEY_NONE                , // 1073741982 0x4000009E SDLK_RETURN2
KEY_NONE                , // 1073741983 0x4000009F SDLK_SEPARATOR
KEY_NONE                , // 1073741984 0x400000A0 SDLK_OUT
KEY_NONE                , // 1073741985 0x400000A1 SDLK_OPER
KEY_NONE                , // 1073741986 0x400000A2 SDLK_CLEARAGAIN
KEY_NONE                , // 1073741987 0x400000A3 SDLK_CRSEL
KEY_NONE                , // 1073741988 0x400000A4 SDLK_EXSEL
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073742000 0x400000B0 SDLK_KP_00
KEY_NONE                , // 1073742001 0x400000B1 SDLK_KP_000
KEY_NONE                , // 1073742002 0x400000B2 SDLK_THOUSANDSSEPARATOR
KEY_NONE                , // 1073742003 0x400000B3 SDLK_DECIMALSEPARATOR
KEY_NONE                , // 1073742004 0x400000B4 SDLK_CURRENCYUNIT
KEY_NONE                , // 1073742005 0x400000B5 SDLK_CURRENCYSUBUNIT
KEY_NONE                , // 1073742006 0x400000B6 SDLK_KP_LEFTPAREN
KEY_NONE                , // 1073742007 0x400000B7 SDLK_KP_RIGHTPAREN
KEY_NONE                , // 1073742008 0x400000B8 SDLK_KP_LEFTBRACE
KEY_NONE                , // 1073742009 0x400000B9 SDLK_KP_RIGHTBRACE
KEY_NONE                , // 1073742010 0x400000BA SDLK_KP_TAB
KEY_NONE                , // 1073742011 0x400000BB SDLK_KP_BACKSPACE
KEY_NONE                , // 1073742012 0x400000BC SDLK_KP_A
KEY_NONE                , // 1073742013 0x400000BD SDLK_KP_B
KEY_NONE                , // 1073742014 0x400000BE SDLK_KP_C
KEY_NONE                , // 1073742015 0x400000BF SDLK_KP_D
KEY_NONE                , // 1073742016 0x400000C0 SDLK_KP_E
KEY_NONE                , // 1073742017 0x400000C1 SDLK_KP_F
KEY_NONE                , // 1073742018 0x400000C2 SDLK_KP_XOR
KEY_NONE                , // 1073742019 0x400000C3 SDLK_KP_POWER
KEY_NONE                , // 1073742020 0x400000C4 SDLK_KP_PERCENT
KEY_NONE                , // 1073742021 0x400000C5 SDLK_KP_LESS
KEY_NONE                , // 1073742022 0x400000C6 SDLK_KP_GREATER
KEY_NONE                , // 1073742023 0x400000C7 SDLK_KP_AMPERSAND
KEY_NONE                , // 1073742024 0x400000C8 SDLK_KP_DBLAMPERSAND
KEY_NONE                , // 1073742025 0x400000C9 SDLK_KP_VERTICALBAR
KEY_NONE                , // 1073742026 0x400000CA SDLK_KP_DBLVERTICALBAR
KEY_NONE                , // 1073742027 0x400000CB SDLK_KP_COLON
KEY_NONE                , // 1073742028 0x400000CC SDLK_KP_HASH
KEY_NONE                , // 1073742029 0x400000CD SDLK_KP_SPACE
KEY_NONE                , // 1073742030 0x400000CE SDLK_KP_AT
KEY_NONE                , // 1073742031 0x400000CF SDLK_KP_EXCLAM
KEY_NONE                , // 1073742032 0x400000D0 SDLK_KP_MEMSTORE
KEY_NONE                , // 1073742033 0x400000D1 SDLK_KP_MEMRECALL
KEY_NONE                , // 1073742034 0x400000D2 SDLK_KP_MEMCLEAR
KEY_NONE                , // 1073742035 0x400000D3 SDLK_KP_MEMADD
KEY_NONE                , // 1073742036 0x400000D4 SDLK_KP_MEMSUBTRACT
KEY_NONE                , // 1073742037 0x400000D5 SDLK_KP_MEMMULTIPLY
KEY_NONE                , // 1073742038 0x400000D6 SDLK_KP_MEMDIVIDE
KEY_NONE                , // 1073742039 0x400000D7 SDLK_KP_PLUSMINUS
KEY_NONE                , // 1073742040 0x400000D8 SDLK_KP_CLEAR
KEY_NONE                , // 1073742041 0x400000D9 SDLK_KP_CLEARENTRY
KEY_NONE                , // 1073742042 0x400000DA SDLK_KP_BINARY
KEY_NONE                , // 1073742043 0x400000DB SDLK_KP_OCTAL
KEY_NONE                , // 1073742044 0x400000DC SDLK_KP_DECIMAL
KEY_NONE                , // 1073742045 0x400000DD SDLK_KP_HEXADECIMAL
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073742048 0x400000E0 SDLK_LCTRL
KEY_LSHIFT                , // 1073742049 0x400000E1 SDLK_LSHIFT
KEY_NONE                , // 1073742050 0x400000E2 SDLK_LALT
KEY_NONE                , // 1073742051 0x400000E3 SDLK_LGUI
KEY_NONE                , // 1073742052 0x400000E4 SDLK_RCTRL
KEY_RSHIFT                , // 1073742053 0x400000E5 SDLK_RSHIFT
KEY_NONE                , // 1073742054 0x400000E6 SDLK_RALT
KEY_NONE                , // 1073742055 0x400000E7 SDLK_RGUI
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE                , // 1073742081 0x40000101 SDLK_MODE
KEY_NONE                , // 1073742082 0x40000102 SDLK_AUDIONEXT
KEY_NONE                , // 1073742083 0x40000103 SDLK_AUDIOPREV
KEY_NONE                , // 1073742084 0x40000104 SDLK_AUDIOSTOP
KEY_NONE                , // 1073742085 0x40000105 SDLK_AUDIOPLAY
KEY_NONE                , // 1073742086 0x40000106 SDLK_AUDIOMUTE
KEY_NONE                , // 1073742087 0x40000107 SDLK_MEDIASELECT
KEY_NONE                , // 1073742088 0x40000108 SDLK_WWW
KEY_NONE                , // 1073742089 0x40000109 SDLK_MAIL
KEY_NONE                , // 1073742090 0x4000010A SDLK_CALCULATOR
KEY_NONE                , // 1073742091 0x4000010B SDLK_COMPUTER
KEY_NONE                , // 1073742092 0x4000010C SDLK_AC_SEARCH
KEY_NONE                , // 1073742093 0x4000010D SDLK_AC_HOME
KEY_NONE                , // 1073742094 0x4000010E SDLK_AC_BACK
KEY_NONE                , // 1073742095 0x4000010F SDLK_AC_FORWARD
KEY_NONE                , // 1073742096 0x40000110 SDLK_AC_STOP
KEY_NONE                , // 1073742097 0x40000111 SDLK_AC_REFRESH
KEY_NONE                , // 1073742098 0x40000112 SDLK_AC_BOOKMARKS
KEY_NONE                , // 1073742099 0x40000113 SDLK_BRIGHTNESSDOWN
KEY_NONE                , // 1073742100 0x40000114 SDLK_BRIGHTNESSUP
KEY_NONE                , // 1073742101 0x40000115 SDLK_DISPLAYSWITCH
KEY_NONE                , // 1073742102 0x40000116 SDLK_KBDILLUMTOGGLE
KEY_NONE                , // 1073742103 0x40000117 SDLK_KBDILLUMDOWN
KEY_NONE                , // 1073742104 0x40000118 SDLK_KBDILLUMUP
KEY_NONE                , // 1073742105 0x40000119 SDLK_EJECT
KEY_NONE                , // 1073742106 0x4000011A SDLK_SLEEP
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
KEY_NONE				, //            0x40000000	
};