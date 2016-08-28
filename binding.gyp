{
	"targets": [
	{
		"target_name": "speex",
		"defines": ["HAVE_CONFIG_H"],
		"sources": [
			"src/Speex.cc", "src/SpeexDecoder.cc", "src/SpeexEncoder.cc", "src/slots.c", "src/util.cc",
			"deps/speex/libspeex/cb_search.c",
			"deps/speex/libspeex/exc_10_32_table.c",
			"deps/speex/libspeex/exc_8_128_table.c",
			"deps/speex/libspeex/filters.c",
			"deps/speex/libspeex/gain_table.c",
			"deps/speex/libspeex/hexc_table.c",
			"deps/speex/libspeex/high_lsp_tables.c",
			"deps/speex/libspeex/lsp.c",
			"deps/speex/libspeex/ltp.c",
			"deps/speex/libspeex/speex.c",
			"deps/speex/libspeex/stereo.c",
			"deps/speex/libspeex/vbr.c",
			"deps/speex/libspeex/vq.c",
			"deps/speex/libspeex/bits.c",
			"deps/speex/libspeex/exc_10_16_table.c",
			"deps/speex/libspeex/exc_20_32_table.c",
			"deps/speex/libspeex/exc_5_256_table.c",
			"deps/speex/libspeex/exc_5_64_table.c",
			"deps/speex/libspeex/gain_table_lbr.c",
			"deps/speex/libspeex/hexc_10_32_table.c",
			"deps/speex/libspeex/lpc.c",
			"deps/speex/libspeex/lsp_tables_nb.c",
			"deps/speex/libspeex/modes.c",
			"deps/speex/libspeex/modes_wb.c",
			"deps/speex/libspeex/nb_celp.c",
			"deps/speex/libspeex/quant_lsp.c",
			"deps/speex/libspeex/sb_celp.c",
			"deps/speex/libspeex/speex_callbacks.c",
			"deps/speex/libspeex/speex_header.c",
			"deps/speex/libspeex/window.c",
			"deps/speex/libspeex/preprocess.c",
			"deps/speex/libspeex/jitter.c",
			"deps/speex/libspeex/mdf.c",
			"deps/speex/libspeex/fftwrap.c",
			"deps/speex/libspeex/filterbank.c",
			"deps/speex/libspeex/resample.c",
			"deps/speex/libspeex/buffer.c",
			"deps/speex/libspeex/scal.c",
			"deps/speex/libspeex/kiss_fft.c",
			"deps/speex/libspeex/kiss_fftr.c",
		],
		"include_dirs": [
			'src',
			'deps/speex/include',
		]
	}
	]
}
