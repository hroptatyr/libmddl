<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="/xsd:schema">
    <xsl:text>/* AUTO-GENERATED, DO NOT MODIFY */&#0010;&#0010;</xsl:text>
    <xsl:text>#if !defined mddl_lite_h_&#0010;</xsl:text>
    <xsl:text>#define mddl_lite_h_&#0010;&#0010;</xsl:text>

    <xsl:apply-templates mode="tdef"/>
    <xsl:text>&#0010;</xsl:text>

    <!-- treat source and sequence specially -->
    <xsl:apply-templates
      select="xsd:element[@name='sequence' or @name='source']"/>

    <!-- treat snap, qref and qsttype specially -->
    <xsl:apply-templates select="xsd:element[@name='snap']"/>
    <xsl:apply-templates select="xsd:element[@name='queryReference']"/>
    <xsl:apply-templates select="xsd:element[@name='queryStatusType']"/>
    <xsl:apply-templates select="xsd:element[@name='mddlQuerySource']"/>
    <xsl:apply-templates select="xsd:element[@name='query']"/>

    <!-- and now the rest -->
    <xsl:apply-templates
      select="xsd:element[not(
              @name='source' or
              @name='sequence' or
              @name='snap' or
              @name='queryReference' or
              @name='queryStatusType' or
              @name='mddlQuerySource' or
              @name='query')]"/>

    <xsl:text>#endif  /* !mddl_lite_h_*/&#0010;</xsl:text>
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

  <xsl:template match="xsd:element[@type and @name]" mode="tdef">
    <xsl:text>typedef </xsl:text>
    <xsl:call-template name="make_type">
      <xsl:with-param name="type" select="@name"/>
    </xsl:call-template>
    <xsl:text> *mddl_</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>_t;&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:element[@type and @name]">
    <xsl:variable name="type" select="@type"/>
    <xsl:variable name="type_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$type"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:call-template name="make_type">
      <xsl:with-param name="type" select="@name"/>
    </xsl:call-template>
    <xsl:text> {</xsl:text>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates
      select="/xsd:schema/xsd:complexType[@name=$type_nons]" mode="porn">
      <xsl:with-param name="indent">
        <xsl:text>&#0009;</xsl:text>
      </xsl:with-param>
    </xsl:apply-templates>
    <xsl:text>};</xsl:text>
    <xsl:text>&#0010;&#0010;</xsl:text>
  </xsl:template>

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
    <xsl:param name="indent"/>

    <xsl:value-of select="$indent"/>
    <xsl:text>/* </xsl:text>
    <xsl:value-of select="name()"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> */&#0010;</xsl:text>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:element[@ref]" mode="porn">
    <xsl:param name="indent"/>
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

    <xsl:if test="$maxocc = 'unbounded'">
      <xsl:value-of select="$indent"/>
      <xsl:text>size_t n</xsl:text>
      <xsl:value-of select="$stem"/>
      <xsl:text>;&#0010;</xsl:text>
    </xsl:if>

    <xsl:value-of select="$indent"/>
    <xsl:call-template name="make_type">
      <xsl:with-param name="type" select="@ref"/>
    </xsl:call-template>

    <xsl:text> </xsl:text>
    <xsl:if test="$maxocc = 'unbounded'">
      <xsl:text>*</xsl:text>
    </xsl:if>
    <xsl:value-of select="$stem"/>
    <xsl:if test="not($maxocc = 'unbounded')">
      <xsl:text>[</xsl:text>
      <xsl:choose>
        <xsl:when test="@maxOccurs">
          <xsl:value-of select="@maxOccurs"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$maxocc"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>;&#0010;</xsl:text>
  </xsl:template>
  <!-- catchalls for when's and other's -->
  <xsl:template match="xsd:element[@ref = 'mddl:when']" mode="porn"/>
  <xsl:template match="xsd:element[@ref = 'mddl:other']" mode="porn"/>

  <xsl:template match="xsd:group[@name]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:param name="maxocc"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:group[@ref]" mode="porn">
    <xsl:param name="indent"/>

    <xsl:variable name="ref" select="@ref"/>
    <xsl:variable name="ref_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:text>&#0010;</xsl:text>
    <xsl:value-of select="$indent"/>
    <xsl:text>/* </xsl:text>
    <xsl:value-of select="$ref"/>
    <xsl:text> */&#0010;</xsl:text>

    <xsl:apply-templates
      select="/xsd:schema/xsd:group[@name=$ref_nons]" mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
      <xsl:with-param name="maxocc" select="@maxOccurs"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence[@maxOccurs]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence" mode="porn">
    <xsl:param name="indent"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:choice[count(*) = 1]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="porn">
      <xsl:with-param name="indent" select="$indent"/>
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:choice" mode="porn">
    <xsl:param name="indent"/>
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:choose>
      <xsl:when test="$maxocc = 'unbounded'">
        <!-- pseudo union, just fuck it -->
        <xsl:apply-templates mode="porn">
          <xsl:with-param name="indent" select="$indent"/>
          <xsl:with-param name="maxocc" select="$maxocc"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:when test="@maxOccurs = 'unbounded'">
        <!-- pseudo union, just fuck it -->
        <xsl:apply-templates mode="porn">
          <xsl:with-param name="indent" select="$indent"/>
          <xsl:with-param name="maxocc" select="@maxOccurs"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>&#0010;</xsl:text>
        <xsl:value-of select="$indent"/>
        <xsl:text>union {&#0010;</xsl:text>
        <xsl:apply-templates mode="porn">
          <xsl:with-param name="indent" select="concat('&#0009;', $indent)"/>
          <xsl:with-param name="maxocc" select="$maxocc"/>
        </xsl:apply-templates>
        <xsl:value-of select="$indent"/>
        <xsl:text>};</xsl:text>
        <xsl:text>&#0010;&#0010;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- special treatment for source/sequence -->
  <xsl:template match="xsd:choice[
                       xsd:element/@ref='mddl:sequence' and
                       xsd:element/@ref='mddl:source' and
                       count(xsd:element) = 2]" mode="porn">
    <xsl:param name="indent"/>

    <xsl:text>&#0010;</xsl:text>
    <xsl:value-of select="$indent"/>
    <xsl:text>/* seq/src */</xsl:text>
    <xsl:text>&#0010;</xsl:text>
    <xsl:value-of select="$indent"/>
    <xsl:text>struct __sequence_s sequence[1];&#0010;</xsl:text>
    <xsl:value-of select="$indent"/>
    <xsl:text>struct __source_s source[1];&#0010;</xsl:text>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <!-- and now even more special treatment due to recurcurcurcursion -->
  <xsl:template match="xsd:choice[
                       xsd:element/@ref='mddl:sequence' and
                       xsd:element/@ref='mddl:source' and
                       count(xsd:element) = 2 and
                       (../../@name='Source' or ../../@name='Sequence')]"
    mode="porn">
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:attribute[@type]" mode="porn">
    <xsl:param name="indent"/>

    <xsl:value-of select="$indent"/>
    <xsl:text>/* ATTR </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:if test="@use">
      <xsl:text> USE:</xsl:text>
      <xsl:value-of select="@use"/>
    </xsl:if>
    <xsl:if test="@default">
      <xsl:text> DEFAULT:</xsl:text>
      <xsl:value-of select="@default"/>
    </xsl:if>
    <xsl:text> */&#0010;</xsl:text>

    <xsl:value-of select="$indent"/>
    <xsl:call-template name="make_type">
      <xsl:with-param name="type">
        <xsl:choose>
          <xsl:when test="@type">
            <xsl:value-of select="@type"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>mdString</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>[1];&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="xsd:attribute[@ref]" mode="porn">
    <xsl:param name="indent"/>
    <xsl:variable name="stem">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="@ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="$indent"/>
    <xsl:text>/* ATTR </xsl:text>
    <xsl:value-of select="@type"/>
    <xsl:if test="@use">
      <xsl:text> USE:</xsl:text>
      <xsl:value-of select="@use"/>
    </xsl:if>
    <xsl:if test="@default">
      <xsl:text> DEFAULT:</xsl:text>
      <xsl:value-of select="@default"/>
    </xsl:if>
    <xsl:text> */&#0010;</xsl:text>

    <xsl:value-of select="$indent"/>
    <xsl:call-template name="make_type">
      <xsl:with-param name="type">
        <xsl:text>mdString</xsl:text>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$stem"/>
    <xsl:text>[1];&#0010;</xsl:text>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="porn"/>
  <xsl:template match="text()" mode="tdef"/>
  <xsl:template match="xsd:*"/>
  <xsl:template match="xsd:*" mode="porn"/>
  <xsl:template match="xsd:*" mode="tdef"/>

</xsl:stylesheet>