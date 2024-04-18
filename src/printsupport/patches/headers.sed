# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2015 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.


# Update of legal information

1 {
  i  /* SPDX-License-Identifier: GPL-3.0-only
  i\\ *
  i\\ * This file is part of LibreMapper.
  i\\ *
  i\\ * Changes:
  i\\ * %DATE% Kai Pastor <dg0yt@darc.de> (OpenOrienteering)
  i\\ * - Adjustment of legal information
  i\\ * - Modifications required for separate compilation:
  i\\ *   - Renaming of selected files, classes, members and macros
  i\\ *   - Adjustment of include statements
  i\\ *   - Removal of Q_XXX_EXPORT
  i\\ */
}
/\*\* *This file is part of.*Qt/ {
: cleanup
  /\*\//! { N; b cleanup }
  s/.*\n//
}

# Renaming of classes, members and macros

s/QPdf/AdvancedPdf/g
s/qpdf/advanced_pdf/
s/QPDF/ADVANCED_PDF/
s/QPRINTENGINE_PDF/PRINTENGINE_ADVANCED_PDF/

# Adjustment of include statements

s/<advanced_pdfwriter.h>/\"advanced_pdfwriter.h\"/
s/qprintengine_pdf/printengine_advanced_pdf/
s/private\/advanced_pdf_p.h/advanced_pdf_p.h/
s/<private\/qfontsubset_p.h>/\"qfontsubset_p.h\"/
s/QtCore\/private/private/
/qfontsubset/!s/\"\(q\w*\)_p.h\"/<private\/\1_p.h>/
/qfontsubset/!s/\"\(private\/q\w*\)_p.h\"/<\1_p.h>/

# Removal of Q_XXX_EXPORT

s/Q_\([A-Z]*\)_EXPORT //
