<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>
  <xsl:key name="attr" match="xsd:attribute" use="@name"/>

  <xsl:include href="autospec-common.xsl"/>

  <xsl:template match="/xsd:schema">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */

%{
typedef struct mddl_attr_s *mddl_attr_t;

typedef enum {
	/* must be first */
	MDDL_ATTR_UNK,

</xsl:text>

    <xsl:apply-templates mode="enum"/>

    <xsl:text>
} mddl_aid_t;

%}
%7bit
%readonly-tables
%struct-type
%define slot-name attr
%define hash-function-name __aid_hash
%define lookup-function-name __aiddify
%null-strings
%enum

struct mddl_attr_s {
	const char *attr;
	mddl_aid_t aid;
};

%%
</xsl:text>

    <xsl:apply-templates mode="lookup"/>
  </xsl:template>

  <xsl:template match="xsd:attribute[@name]" mode="enum">
    <!-- uniquify -->
    <xsl:if test="generate-id() = generate-id(key('attr', @name))">
      <xsl:text>&#0009;</xsl:text>
      <xsl:call-template name="make_attr">
        <xsl:with-param name="name">
          <xsl:value-of select="@name"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>,&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="xsd:attribute[@name]" mode="lookup">
    <!-- uniquify -->
    <xsl:if test="generate-id() = generate-id(key('attr', @name))">
      <xsl:value-of select="@name"/>
      <xsl:text>, </xsl:text>
      <xsl:text>&#0009;</xsl:text>
      <xsl:call-template name="make_attr">
        <xsl:with-param name="name">
          <xsl:value-of select="@name"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="enum"/>
  <xsl:template match="text()" mode="lookup"/>

</xsl:stylesheet>
