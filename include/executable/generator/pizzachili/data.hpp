//  Copyright (c) 2019 Jonas Ellert
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#pragma once

struct pc_data_type {
  constexpr static uint64_t instances = 18;

  const std::string dl_texts = "http://pizzachili.dcc.uchile.cl/texts/";
  const std::string dl_rep_artificial =
      "http://pizzachili.dcc.uchile.cl/repcorpus/artificial/";
  const std::string dl_rep_real =
      "http://pizzachili.dcc.uchile.cl/repcorpus/real/";

  const std::string archive_suffix = ".gz";

  const std::string file_dna = "dna";
  const std::string file_english = "english";
  const std::string file_pitches = "pitches";
  const std::string file_proteins = "proteins";
  const std::string file_sources = "sources";
  const std::string file_xml = "dblp.xml";

  const std::string file_fib = "fib41";
  const std::string file_rs = "rs.13";
  const std::string file_tm = "tm29";

  const std::string file_ecoli = "Escherichia_Coli";
  const std::string file_cere = "cere";
  const std::string file_coreutils = "coreutils";
  const std::string file_einstein_de = "einstein.de.txt";
  const std::string file_einstein_en = "einstein.en.txt";
  const std::string file_influenza = "influenza";
  const std::string file_kernel = "kernel";
  const std::string file_para = "para";
  const std::string file_leaders = "world_leaders";

  const std::string dl_dna = dl_texts + "dna/" + file_dna + archive_suffix;
  const std::string dl_english =
      dl_texts + "nlang/" + file_english + archive_suffix;
  const std::string dl_pitches =
      dl_texts + "music/" + file_pitches + archive_suffix;
  const std::string dl_proteins =
      dl_texts + "protein/" + file_proteins + archive_suffix;
  const std::string dl_sources =
      dl_texts + "code/" + file_sources + archive_suffix;
  const std::string dl_xml = dl_texts + "xml/" + file_xml + archive_suffix;

  const std::string dl_fib = dl_rep_artificial + file_fib + archive_suffix;
  const std::string dl_rs = dl_rep_artificial + file_rs + archive_suffix;
  const std::string dl_tm = dl_rep_artificial + file_tm + archive_suffix;

  const std::string dl_ecoli = dl_rep_real + file_ecoli + archive_suffix;
  const std::string dl_cere = dl_rep_real + file_cere + archive_suffix;
  const std::string dl_coreutils =
      dl_rep_real + file_coreutils + archive_suffix;
  const std::string dl_einstein_de =
      dl_rep_real + file_einstein_de + archive_suffix;
  const std::string dl_einstein_en =
      dl_rep_real + file_einstein_en + archive_suffix;
  const std::string dl_influenza =
      dl_rep_real + file_influenza + archive_suffix;
  const std::string dl_kernel = dl_rep_real + file_kernel + archive_suffix;
  const std::string dl_para = dl_rep_real + file_para + archive_suffix;
  const std::string dl_leaders = dl_rep_real + file_leaders + archive_suffix;

  const std::string file_list[instances] = {
      file_dna,         file_english,     file_pitches,   file_proteins,
      file_sources,     file_xml,         file_fib,       file_rs,
      file_tm,          file_ecoli,       file_cere,      file_coreutils,
      file_einstein_de, file_einstein_en, file_influenza, file_kernel,
      file_para,        file_leaders};

  const std::string archive_list[instances] = {
      file_dna + archive_suffix,         file_english + archive_suffix,
      file_pitches + archive_suffix,     file_proteins + archive_suffix,
      file_sources + archive_suffix,     file_xml + archive_suffix,
      file_fib + archive_suffix,         file_rs + archive_suffix,
      file_tm + archive_suffix,          file_ecoli + archive_suffix,
      file_cere + archive_suffix,        file_coreutils + archive_suffix,
      file_einstein_de + archive_suffix, file_einstein_en + archive_suffix,
      file_influenza + archive_suffix,   file_kernel + archive_suffix,
      file_para + archive_suffix,        file_leaders + archive_suffix};

  const std::string dl_list[instances] = {
      dl_dna,    dl_english,   dl_pitches,     dl_proteins,    dl_sources,
      dl_xml,    dl_fib,       dl_rs,          dl_tm,          dl_ecoli,
      dl_cere,   dl_coreutils, dl_einstein_de, dl_einstein_en, dl_influenza,
      dl_kernel, dl_para,      dl_leaders};

} pc_data;