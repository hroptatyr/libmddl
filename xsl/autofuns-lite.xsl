<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>
  <xsl:param name="hdr"/>

  <xsl:template match="/xsd:schema">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */&#0010;&#0010;</xsl:text>
    <xsl:apply-templates select="xsd:element"/>
  </xsl:template>

  <xsl:template name="make_stem">
    <xsl:param name="type"/>

    <xsl:choose>
      <xsl:when test="starts-with($type, 'mddl:')">
        <xsl:value-of select="substring($type, 6)"/>
      </xsl:when>
      <xsl:when test="starts-with($type, 'xsd:')">
        <xsl:value-of select="substring($type, 5)"/>
      </xsl:when>
      <xsl:when test="starts-with($type, 'xml:')">
        <xsl:value-of select="substring($type, 5)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$type"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="make_type">
    <xsl:param name="type"/>

    <xsl:text>struct __</xsl:text>
    <xsl:call-template name="make_stem">
      <xsl:with-param name="type" select="$type"/>
    </xsl:call-template>
    <xsl:text>_s</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:element[@type and @name]">
    <xsl:variable name="type_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="@type"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:apply-templates
      select="/xsd:schema/xsd:complexType[@name=$type_nons]" mode="porn">
      <xsl:with-param name="super" select="@name"/>
    </xsl:apply-templates>
    <xsl:text>&#0010;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:element[contains(@name, 'Group')]"/>

  <!-- do not process these -->
  <xsl:template match="xsd:simpleType"/>
  <xsl:template match="xsd:element[@name = 'mdMath']"/>
  <xsl:template match="xsd:element[@name = 'mdDecimal']"/>
  <xsl:template match="xsd:element[@name = 'mdDateTime']"/>
  <xsl:template match="xsd:element[@name = 'mdString']"/>
  <xsl:template match="xsd:element[@name = 'mdNonNegativeDecimal']"/>
  <xsl:template match="xsd:element[@name = 'mdInteger']"/>
  <xsl:template match="xsd:element[@name = 'mdBoolean']"/>
  <xsl:template match="xsd:element[@name = 'mdDuration']"/>
  <xsl:template match="xsd:element[@name = 'mdUri']"/>

  <xsl:template match="xsd:complexType" mode="porn">
    <xsl:param name="super"/>

    <xsl:text>/* </xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$super"/>
    <xsl:text> */&#0010;</xsl:text>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="super" select="$super"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:element[@ref]" mode="porn">
    <xsl:param name="super"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:variable
      name="type"
      select="../../xsd:annotation/xsd:appinfo/mddl:schema-classification/@type"/>
    <xsl:variable name="stem">
      <xsl:choose>
        <xsl:when test="$type">
          <xsl:value-of select="$type"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="make_stem">
            <xsl:with-param name="type" select="@ref"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="super_type">
      <xsl:text>mddl_</xsl:text>
      <xsl:value-of select="$super"/>
      <xsl:text>_t</xsl:text>
    </xsl:variable>
    <xsl:variable name="return_type">
      <xsl:text>mddl_</xsl:text>
      <xsl:value-of select="$stem"/>
      <xsl:text>_t</xsl:text>
    </xsl:variable>

    <xsl:if test="$maxocc = 'unbounded'">
      <xsl:choose>
        <xsl:when test="$hdr">
          <xsl:text>DECLF </xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>DEFUN </xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:value-of select="$return_type"/>
      <xsl:choose>
        <xsl:when test="$hdr">
          <xsl:text> </xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#0010;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>

      <xsl:text>mddl_</xsl:text>
      <xsl:value-of select="$super"/>
      <xsl:text>_add_</xsl:text>
      <xsl:value-of select="$stem"/>

      <xsl:text>(</xsl:text>
      <xsl:value-of select="$super_type"/>
      <xsl:text> to</xsl:text>
      <xsl:text>)</xsl:text>

      <xsl:choose>
        <xsl:when test="$hdr">
          <xsl:text>;&#0010;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>&#0010;{&#0010;</xsl:text>
          <xsl:text>&#0009;ADDF(</xsl:text>	
          <xsl:value-of select="$stem"/>
          <xsl:text>);&#0010;</xsl:text>
          <xsl:text>}&#0010;&#0010;</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>
  <!-- catchalls for when's and other's -->
  <xsl:template match="xsd:element[@ref = 'mddl:when']" mode="porn"/>
  <xsl:template match="xsd:element[@ref = 'mddl:other']" mode="porn"/>

  <xsl:template match="xsd:group[@name]" mode="porn">
    <xsl:param name="super"/>
    <xsl:param name="maxocc"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="super" select="$super"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:group[@ref]" mode="porn">
    <xsl:param name="super"/>

    <xsl:variable name="ref" select="@ref"/>
    <xsl:variable name="ref_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:apply-templates
      select="/xsd:schema/xsd:group[@name=$ref_nons]" mode="porn">
      <xsl:with-param name="super" select="$super"/>
      <xsl:with-param name="maxocc" select="@maxOccurs"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence[@maxOccurs]" mode="porn">
    <xsl:param name="super"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="super" select="$super"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence" mode="porn">
    <xsl:param name="super"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="super" select="$super"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:choice" mode="porn">
    <xsl:param name="super"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="super" select="$super"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="porn"/>
  <xsl:template match="xsd:*"/>
  <xsl:template match="xsd:*" mode="porn"/>

</xsl:stylesheet>
