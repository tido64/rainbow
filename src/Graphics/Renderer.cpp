// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Common/SpriteVertex.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Shaders/Shaders.h"

bool Renderer::init()
{
#ifdef __glew_h__
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		R_ERROR("[Rainbow] Failed to initialise GLEW: %s\n", glewGetErrorString(err));
		return false;
	}
#endif

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef RAINBOW_SDL
	vertex_shader = load_shader(GL_VERTEX_SHADER, fixed2d_vsh);
#else
	vertex_shader = load_shader(GL_VERTEX_SHADER, "Shaders/Fixed2D.vsh");
#endif
	if (!vertex_shader)
		return false;

#ifndef RAINBOW_SDL
	fragment_shader = load_shader(GL_FRAGMENT_SHADER, fixed2d_fsh);
#else
	fragment_shader = load_shader(GL_FRAGMENT_SHADER, "Shaders/Fixed2D.fsh");
#endif
	if (!fragment_shader)
		return false;

	pipeline = new Pipeline();
	pipeline->attach_shader(vertex_shader);
	pipeline->attach_shader(fragment_shader);
	if (!pipeline->link())
		return false;
	pipeline->use();

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1536 * sizeof(unsigned short), default_indices, GL_STATIC_DRAW);

	return glGetError() == GL_NO_ERROR;
}

void Renderer::release()
{
	glUseProgram(0);
	delete pipeline;
	delete fragment_shader;
	delete vertex_shader;
}

void Renderer::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::resize(const unsigned int width, const unsigned int height)
{
	const float u = 2.0f / width;
	const float v = 2.0f / height;
	const float ortho[] = {
		   u, 0.0f,  0.0f, -1.0f,
		0.0f,    v,  0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,  0.0f,
		0.0f, 0.0f,  0.0f,  1.0f
	};

	R_ASSERT(pipeline, "Pipeline uninitialised");
	glUniformMatrix4fv(glGetUniformLocation(*pipeline, "mvp_matrix"), 1, GL_FALSE, ortho);
	glViewport(0, 0, width, height);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to initialise OpenGL viewport");
}

void Renderer::attach_pipeline(const Pipeline &program)
{
	glUseProgram(program);
}

void Renderer::detach_pipeline()
{
	glUseProgram(*pipeline);
}

void Renderer::bind_buffer(const unsigned int buffer)
{
	static unsigned int bound = 0;
	if (buffer == bound)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	bound = buffer;
}

void Renderer::create_buffer(unsigned int &buffer, unsigned int &array_object)
{
	glGenBuffers(1, &buffer);

#ifndef RAINBOW_ANDROID

	bind_buffer(buffer);

	glGenVertexArrays(1, &array_object);
	glBindVertexArray(array_object);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	glVertexAttribPointer(Pipeline::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex), 0);
	glEnableVertexAttribArray(Pipeline::COLOR_LOCATION);

	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), SpriteVertex::tx_offset);
	glEnableVertexAttribArray(Pipeline::TEXCOORD_LOCATION);

	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex), SpriteVertex::vx_offset);
	glEnableVertexAttribArray(Pipeline::VERTEX_LOCATION);

	glBindVertexArray(0);

#endif
}

void Renderer::delete_buffer(const unsigned int buffer, const unsigned int array_object)
{
#ifndef RAINBOW_ANDROID
	glDeleteVertexArrays(1, &array_object);
#endif
	glDeleteBuffers(1, &buffer);
}

void Renderer::draw_buffer(const unsigned int array_object, const unsigned int count)
{
#ifndef RAINBOW_ANDROID

	glBindVertexArray(array_object);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);

#else

	bind_buffer(array_object);

	glVertexAttribPointer(Pipeline::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex), 0);
	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), SpriteVertex::tx_offset);
	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex), SpriteVertex::vx_offset);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);

#endif

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw buffer");
}

void Renderer::draw_elements(const SpriteVertex *vertices, const unsigned int count)
{
	bind_buffer(0);

	glVertexAttribPointer(Pipeline::COLOR_LOCATION, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SpriteVertex), &vertices->color);
	glVertexAttribPointer(Pipeline::TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), &vertices->texcoord);
	glVertexAttribPointer(Pipeline::VERTEX_LOCATION, 2, GL_FLOAT, GL_TRUE, sizeof(SpriteVertex), &vertices->position);

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to draw elements");
}

Shader* Renderer::load_shader(const unsigned int type, const char *const src_path)
{
#ifndef RAINBOW_SDL
	const char *source = src_path;
#else
	Data source(src_path);
	if (!source)
		return nullptr;
#endif

	Shader *shader = new Shader(type, source);
	if (!*shader)
	{
		delete shader;
		shader = nullptr;
		R_ASSERT(shader, "Failed to compile shader");
	}
	return shader;
}

void Renderer::update_buffer(const unsigned int buffer, const unsigned int size, const void *data)
{
	bind_buffer(buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	bind_buffer(0);

	R_ASSERT(glGetError() == GL_NO_ERROR, "Failed to update buffer");
}

unsigned int Renderer::index_buffer = 0;
Pipeline *Renderer::pipeline = nullptr;
Shader *Renderer::vertex_shader = nullptr;
Shader *Renderer::fragment_shader = nullptr;
const unsigned short Renderer::default_indices[] = {
	  0,   1,   2,   2,   3,   0,   4,   5,   6,   6,   7,   4,
	  8,   9,  10,  10,  11,   8,  12,  13,  14,  14,  15,  12,
	 16,  17,  18,  18,  19,  16,  20,  21,  22,  22,  23,  20,
	 24,  25,  26,  26,  27,  24,  28,  29,  30,  30,  31,  28,
	 32,  33,  34,  34,  35,  32,  36,  37,  38,  38,  39,  36,
	 40,  41,  42,  42,  43,  40,  44,  45,  46,  46,  47,  44,
	 48,  49,  50,  50,  51,  48,  52,  53,  54,  54,  55,  52,
	 56,  57,  58,  58,  59,  56,  60,  61,  62,  62,  63,  60,
	 64,  65,  66,  66,  67,  64,  68,  69,  70,  70,  71,  68,
	 72,  73,  74,  74,  75,  72,  76,  77,  78,  78,  79,  76,
	 80,  81,  82,  82,  83,  80,  84,  85,  86,  86,  87,  84,
	 88,  89,  90,  90,  91,  88,  92,  93,  94,  94,  95,  92,
	 96,  97,  98,  98,  99,  96, 100, 101, 102, 102, 103, 100,
	104, 105, 106, 106, 107, 104, 108, 109, 110, 110, 111, 108,
	112, 113, 114, 114, 115, 112, 116, 117, 118, 118, 119, 116,
	120, 121, 122, 122, 123, 120, 124, 125, 126, 126, 127, 124,
	128, 129, 130, 130, 131, 128, 132, 133, 134, 134, 135, 132,
	136, 137, 138, 138, 139, 136, 140, 141, 142, 142, 143, 140,
	144, 145, 146, 146, 147, 144, 148, 149, 150, 150, 151, 148,
	152, 153, 154, 154, 155, 152, 156, 157, 158, 158, 159, 156,
	160, 161, 162, 162, 163, 160, 164, 165, 166, 166, 167, 164,
	168, 169, 170, 170, 171, 168, 172, 173, 174, 174, 175, 172,
	176, 177, 178, 178, 179, 176, 180, 181, 182, 182, 183, 180,
	184, 185, 186, 186, 187, 184, 188, 189, 190, 190, 191, 188,
	192, 193, 194, 194, 195, 192, 196, 197, 198, 198, 199, 196,
	200, 201, 202, 202, 203, 200, 204, 205, 206, 206, 207, 204,
	208, 209, 210, 210, 211, 208, 212, 213, 214, 214, 215, 212,
	216, 217, 218, 218, 219, 216, 220, 221, 222, 222, 223, 220,
	224, 225, 226, 226, 227, 224, 228, 229, 230, 230, 231, 228,
	232, 233, 234, 234, 235, 232, 236, 237, 238, 238, 239, 236,
	240, 241, 242, 242, 243, 240, 244, 245, 246, 246, 247, 244,
	248, 249, 250, 250, 251, 248, 252, 253, 254, 254, 255, 252,
	256, 257, 258, 258, 259, 256, 260, 261, 262, 262, 263, 260,
	264, 265, 266, 266, 267, 264, 268, 269, 270, 270, 271, 268,
	272, 273, 274, 274, 275, 272, 276, 277, 278, 278, 279, 276,
	280, 281, 282, 282, 283, 280, 284, 285, 286, 286, 287, 284,
	288, 289, 290, 290, 291, 288, 292, 293, 294, 294, 295, 292,
	296, 297, 298, 298, 299, 296, 300, 301, 302, 302, 303, 300,
	304, 305, 306, 306, 307, 304, 308, 309, 310, 310, 311, 308,
	312, 313, 314, 314, 315, 312, 316, 317, 318, 318, 319, 316,
	320, 321, 322, 322, 323, 320, 324, 325, 326, 326, 327, 324,
	328, 329, 330, 330, 331, 328, 332, 333, 334, 334, 335, 332,
	336, 337, 338, 338, 339, 336, 340, 341, 342, 342, 343, 340,
	344, 345, 346, 346, 347, 344, 348, 349, 350, 350, 351, 348,
	352, 353, 354, 354, 355, 352, 356, 357, 358, 358, 359, 356,
	360, 361, 362, 362, 363, 360, 364, 365, 366, 366, 367, 364,
	368, 369, 370, 370, 371, 368, 372, 373, 374, 374, 375, 372,
	376, 377, 378, 378, 379, 376, 380, 381, 382, 382, 383, 380,
	384, 385, 386, 386, 387, 384, 388, 389, 390, 390, 391, 388,
	392, 393, 394, 394, 395, 392, 396, 397, 398, 398, 399, 396,
	400, 401, 402, 402, 403, 400, 404, 405, 406, 406, 407, 404,
	408, 409, 410, 410, 411, 408, 412, 413, 414, 414, 415, 412,
	416, 417, 418, 418, 419, 416, 420, 421, 422, 422, 423, 420,
	424, 425, 426, 426, 427, 424, 428, 429, 430, 430, 431, 428,
	432, 433, 434, 434, 435, 432, 436, 437, 438, 438, 439, 436,
	440, 441, 442, 442, 443, 440, 444, 445, 446, 446, 447, 444,
	448, 449, 450, 450, 451, 448, 452, 453, 454, 454, 455, 452,
	456, 457, 458, 458, 459, 456, 460, 461, 462, 462, 463, 460,
	464, 465, 466, 466, 467, 464, 468, 469, 470, 470, 471, 468,
	472, 473, 474, 474, 475, 472, 476, 477, 478, 478, 479, 476,
	480, 481, 482, 482, 483, 480, 484, 485, 486, 486, 487, 484,
	488, 489, 490, 490, 491, 488, 492, 493, 494, 494, 495, 492,
	496, 497, 498, 498, 499, 496, 500, 501, 502, 502, 503, 500,
	504, 505, 506, 506, 507, 504, 508, 509, 510, 510, 511, 508,
	512, 513, 514, 514, 515, 512, 516, 517, 518, 518, 519, 516,
	520, 521, 522, 522, 523, 520, 524, 525, 526, 526, 527, 524,
	528, 529, 530, 530, 531, 528, 532, 533, 534, 534, 535, 532,
	536, 537, 538, 538, 539, 536, 540, 541, 542, 542, 543, 540,
	544, 545, 546, 546, 547, 544, 548, 549, 550, 550, 551, 548,
	552, 553, 554, 554, 555, 552, 556, 557, 558, 558, 559, 556,
	560, 561, 562, 562, 563, 560, 564, 565, 566, 566, 567, 564,
	568, 569, 570, 570, 571, 568, 572, 573, 574, 574, 575, 572,
	576, 577, 578, 578, 579, 576, 580, 581, 582, 582, 583, 580,
	584, 585, 586, 586, 587, 584, 588, 589, 590, 590, 591, 588,
	592, 593, 594, 594, 595, 592, 596, 597, 598, 598, 599, 596,
	600, 601, 602, 602, 603, 600, 604, 605, 606, 606, 607, 604,
	608, 609, 610, 610, 611, 608, 612, 613, 614, 614, 615, 612,
	616, 617, 618, 618, 619, 616, 620, 621, 622, 622, 623, 620,
	624, 625, 626, 626, 627, 624, 628, 629, 630, 630, 631, 628,
	632, 633, 634, 634, 635, 632, 636, 637, 638, 638, 639, 636,
	640, 641, 642, 642, 643, 640, 644, 645, 646, 646, 647, 644,
	648, 649, 650, 650, 651, 648, 652, 653, 654, 654, 655, 652,
	656, 657, 658, 658, 659, 656, 660, 661, 662, 662, 663, 660,
	664, 665, 666, 666, 667, 664, 668, 669, 670, 670, 671, 668,
	672, 673, 674, 674, 675, 672, 676, 677, 678, 678, 679, 676,
	680, 681, 682, 682, 683, 680, 684, 685, 686, 686, 687, 684,
	688, 689, 690, 690, 691, 688, 692, 693, 694, 694, 695, 692,
	696, 697, 698, 698, 699, 696, 700, 701, 702, 702, 703, 700,
	704, 705, 706, 706, 707, 704, 708, 709, 710, 710, 711, 708,
	712, 713, 714, 714, 715, 712, 716, 717, 718, 718, 719, 716,
	720, 721, 722, 722, 723, 720, 724, 725, 726, 726, 727, 724,
	728, 729, 730, 730, 731, 728, 732, 733, 734, 734, 735, 732,
	736, 737, 738, 738, 739, 736, 740, 741, 742, 742, 743, 740,
	744, 745, 746, 746, 747, 744, 748, 749, 750, 750, 751, 748,
	752, 753, 754, 754, 755, 752, 756, 757, 758, 758, 759, 756,
	760, 761, 762, 762, 763, 760, 764, 765, 766, 766, 767, 764,
	768, 769, 770, 770, 771, 768, 772, 773, 774, 774, 775, 772,
	776, 777, 778, 778, 779, 776, 780, 781, 782, 782, 783, 780,
	784, 785, 786, 786, 787, 784, 788, 789, 790, 790, 791, 788,
	792, 793, 794, 794, 795, 792, 796, 797, 798, 798, 799, 796,
	800, 801, 802, 802, 803, 800, 804, 805, 806, 806, 807, 804,
	808, 809, 810, 810, 811, 808, 812, 813, 814, 814, 815, 812,
	816, 817, 818, 818, 819, 816, 820, 821, 822, 822, 823, 820,
	824, 825, 826, 826, 827, 824, 828, 829, 830, 830, 831, 828,
	832, 833, 834, 834, 835, 832, 836, 837, 838, 838, 839, 836,
	840, 841, 842, 842, 843, 840, 844, 845, 846, 846, 847, 844,
	848, 849, 850, 850, 851, 848, 852, 853, 854, 854, 855, 852,
	856, 857, 858, 858, 859, 856, 860, 861, 862, 862, 863, 860,
	864, 865, 866, 866, 867, 864, 868, 869, 870, 870, 871, 868,
	872, 873, 874, 874, 875, 872, 876, 877, 878, 878, 879, 876,
	880, 881, 882, 882, 883, 880, 884, 885, 886, 886, 887, 884,
	888, 889, 890, 890, 891, 888, 892, 893, 894, 894, 895, 892,
	896, 897, 898, 898, 899, 896, 900, 901, 902, 902, 903, 900,
	904, 905, 906, 906, 907, 904, 908, 909, 910, 910, 911, 908,
	912, 913, 914, 914, 915, 912, 916, 917, 918, 918, 919, 916,
	920, 921, 922, 922, 923, 920, 924, 925, 926, 926, 927, 924,
	928, 929, 930, 930, 931, 928, 932, 933, 934, 934, 935, 932,
	936, 937, 938, 938, 939, 936, 940, 941, 942, 942, 943, 940,
	944, 945, 946, 946, 947, 944, 948, 949, 950, 950, 951, 948,
	952, 953, 954, 954, 955, 952, 956, 957, 958, 958, 959, 956,
	960, 961, 962, 962, 963, 960, 964, 965, 966, 966, 967, 964,
	968, 969, 970, 970, 971, 968, 972, 973, 974, 974, 975, 972,
	976, 977, 978, 978, 979, 976, 980, 981, 982, 982, 983, 980,
	984, 985, 986, 986, 987, 984, 988, 989, 990, 990, 991, 988,
	992, 993, 994, 994, 995, 992, 996, 997, 998, 998, 999, 996,
	1000, 1001, 1002, 1002, 1003, 1000, 1004, 1005, 1006, 1006, 1007, 1004,
	1008, 1009, 1010, 1010, 1011, 1008, 1012, 1013, 1014, 1014, 1015, 1012,
	1016, 1017, 1018, 1018, 1019, 1016, 1020, 1021, 1022, 1022, 1023, 1020
};
