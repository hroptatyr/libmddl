<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>
  <xsl:key name="special" match="slot" use="@class"/>

  <xsl:include href="autospec-common.xsl"/>

  <xsl:template match="spec">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */

%{
typedef struct mddl_tag_s *mddl_tag_t;

typedef enum {
	/* must be first */
	MDDL_TAG_UNK,

</xsl:text>

    <xsl:apply-templates mode="enum"/>
    <xsl:apply-templates select="struct/slot[@class]" mode="enum"/>

    <xsl:text>
} mddl_tid_t;

%}
%7bit
%readonly-tables
%struct-type
%define slot-name tag
%define hash-function-name __tid_hash
%define lookup-function-name __tiddify
%null-strings
%enum

struct mddl_tag_s {
	const char *tag;
	mddl_tid_t tid;
};

%%
</xsl:text>

    <xsl:apply-templates mode="lookup"/>
    <xsl:apply-templates select="struct/slot[@class]" mode="lookup"/>

    <!-- custom bollocks that we erroneously created -->
    <xsl:text>/* compat */
mdPrice,	MDDL_TAG_mdDecimal
mdAmount,	MDDL_TAG_mdDecimal
mdRate,	MDDL_TAG_mdDecimal
</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="enum">
    <xsl:text>&#0009;</xsl:text>
    <xsl:call-template name="make_tag">
      <xsl:with-param name="name">
        <xsl:value-of select="@slot"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>,&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="struct" mode="lookup">
    <xsl:value-of select="@slot"/>
    <xsl:text>, </xsl:text>
    <xsl:text>&#0009;</xsl:text>
    <xsl:call-template name="make_tag">
      <xsl:with-param name="name">
        <xsl:value-of select="@slot"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="slot[@class]" mode="enum">
    <!-- uniquify -->
    <xsl:if test="generate-id() = generate-id(key('special', @class))">
      <xsl:text>&#0009;</xsl:text>
      <xsl:call-template name="make_tag">
        <xsl:with-param name="name">
          <xsl:value-of select="@class"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>,&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="slot[@class]" mode="lookup">
    <!-- uniquify -->
    <xsl:if test="generate-id() = generate-id(key('special', @class))">
      <xsl:value-of select="@class"/>
      <xsl:text>, </xsl:text>
      <xsl:text>&#0009;</xsl:text>
      <xsl:call-template name="make_tag">
        <xsl:with-param name="name">
          <xsl:value-of select="@class"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="enum"/>
  <xsl:template match="text()" mode="lookup"/>
  <xsl:template match="*"/>
  <xsl:template match="*" mode="enum"/>
  <xsl:template match="*" mode="lookup"/>

</xsl:stylesheet>
