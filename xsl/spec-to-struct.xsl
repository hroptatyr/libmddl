<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xsd="http://www.w3.org/2001/XMLSchema"
  xmlns:mddl="http://www.mddl.org/mddl/3.0-beta"
  xmlns:xlink="http://www.w3.org/1999/xlink"
  xmlns:str="http://exslt.org/strings"
  version="1.0">

  <xsl:output method="xml" indent="yes"/>

  <xsl:include href="autospec-common.xsl"/>


  <xsl:template name="make_struct">
    <xsl:param name="type"/>
    <xsl:param name="slot"/>
    <xsl:param name="mult"/>

    <xsl:element name="struct">
      <xsl:attribute name="type">
        <xsl:value-of select="$type"/>
      </xsl:attribute>
      <xsl:attribute name="slot">
        <xsl:value-of select="$slot"/>
      </xsl:attribute>
      <xsl:attribute name="mult">
        <xsl:value-of select="$mult"/>
      </xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template name="make_slot">
    <xsl:param name="type"/>
    <xsl:param name="class"/>
    <xsl:param name="slot"/>
    <xsl:param name="use"/>
    <xsl:param name="default"/>

    <xsl:element name="slot">
      <xsl:attribute name="type">
        <xsl:value-of select="$type"/>
      </xsl:attribute>
      <xsl:attribute name="slot">
        <xsl:value-of select="$slot"/>
      </xsl:attribute>
      <xsl:attribute name="class">
        <xsl:choose>
          <xsl:when test="$class">
            <xsl:value-of select="$class"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$type"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
      <xsl:if test="$use">
        <xsl:attribute name="attr">
          <xsl:value-of select="$use"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="$default">
        <xsl:attribute name="default">
          <xsl:value-of select="$default"/>
        </xsl:attribute>
      </xsl:if>
    </xsl:element>
  </xsl:template>

  <xsl:template match="/xsd:schema">
    <xsl:element name="spec">
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

      <!-- finally them unions and shite -->
      <xsl:apply-templates select="xsd:simpleType"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsd:element[@type and @name]">
    <xsl:variable name="type" select="@type"/>
    <xsl:variable name="type_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$type"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:element name="struct">
      <xsl:attribute name="type">
        <xsl:call-template name="make_type">
          <xsl:with-param name="type" select="@name"/>
        </xsl:call-template>
      </xsl:attribute>
      <xsl:attribute name="slot">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:apply-templates
        select="/xsd:schema/xsd:complexType[@name=$type_nons]" mode="conv">
      </xsl:apply-templates>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsd:simpleType[xsd:restriction]">
    <xsl:element name="enum">
      <xsl:attribute name="type">
        <xsl:text>enum mddl_</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text>_e</xsl:text>
      </xsl:attribute>
      <xsl:attribute name="slot">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsd:restriction">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xsd:enumeration">
    <xsl:element name="value">
      <xsl:attribute name="slot">
        <xsl:value-of select="@value"/>
      </xsl:attribute>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsd:simpleType[xsd:union]">
    <xsl:element name="union">
      <xsl:attribute name="type">
        <xsl:text>union mddl_</xsl:text>
        <xsl:value-of select="@name"/>
        <xsl:text>_u</xsl:text>
      </xsl:attribute>
      <xsl:attribute name="slot">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsd:union">
    <xsl:variable name="m" select="str:split(@memberTypes)"/>
    <xsl:for-each select="$m">
      <xsl:element name="slot">
        <xsl:attribute name="type">
          <xsl:text>mddl_</xsl:text>
          <xsl:call-template name="make_stem">
            <xsl:with-param name="type" select="."/>
          </xsl:call-template>
          <xsl:text>_t</xsl:text>
        </xsl:attribute>
        <xsl:attribute name="slot">
          <xsl:call-template name="make_stem">
            <xsl:with-param name="type" select="."/>
          </xsl:call-template>
        </xsl:attribute>
      </xsl:element>
    </xsl:for-each>
  </xsl:template>

  <!-- do not process these -->
  <xsl:template
    match="xsd:element[
           substring(@name, string-length(@name) - 4, 5) = 'Group']"/>
  <xsl:template match="xsd:element[@name = 'mdMath']"/>
  <xsl:template match="xsd:element[@name = 'mdDecimal']"/>
  <xsl:template match="xsd:element[@name = 'mdDateTime']"/>
  <xsl:template match="xsd:element[@name = 'mdString']"/>
  <xsl:template match="xsd:element[@name = 'mdNonNegativeDecimal']"/>
  <xsl:template match="xsd:element[@name = 'mdInteger']"/>
  <xsl:template match="xsd:element[@name = 'mdBoolean']"/>
  <xsl:template match="xsd:element[@name = 'mdDuration']"/>
  <xsl:template match="xsd:element[@name = 'mdUri']"/>

  <xsl:template match="xsd:element[@name = 'mdMath']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdDecimal']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdDateTime']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdString']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdNonNegativeDecimal']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdInteger']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdBoolean']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdDuration']" mode="tdef"/>
  <xsl:template match="xsd:element[@name = 'mdUri']" mode="tdef"/>

  <xsl:template match="xsd:complexType" mode="conv">
    <xsl:comment>
    <xsl:value-of select="name()"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    </xsl:comment>

    <xsl:apply-templates mode="conv"/>
  </xsl:template>

  <xsl:template match="xsd:element[@ref]" mode="conv">
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
    <xsl:variable name="__type">
      <xsl:call-template name="make_type">
        <xsl:with-param name="type" select="@ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:choose>
      <xsl:when test="starts-with($__type, 'mddl_')">
        <xsl:call-template name="make_slot">
          <xsl:with-param name="class">
            <xsl:call-template name="make_stem">
              <xsl:with-param name="type" select="@ref"/>
            </xsl:call-template>
          </xsl:with-param>
          <xsl:with-param name="type" select="$__type"/>
          <xsl:with-param name="slot" select="$stem"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="substring(@ref, string-length(@ref) - 4, 5) = 'Group'">
        <!-- skip groups -->
        <xsl:comment>
          <xsl:value-of select="@ref"/>
          <xsl:text> omitted</xsl:text>
        </xsl:comment>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="make_struct">
          <xsl:with-param name="type" select="$__type"/>
          <xsl:with-param name="slot" select="$stem"/>
          <xsl:with-param name="mult">
            <xsl:choose>
              <xsl:when test="$maxocc = 'unbounded'">
                <xsl:text>*</xsl:text>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$maxocc"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>
  <!-- catchalls for when's and other's -->
  <xsl:template match="xsd:element[@ref = 'mddl:when']" mode="conv"/>
  <xsl:template match="xsd:element[@ref = 'mddl:other']" mode="conv"/>

  <xsl:template match="xsd:group[@name]" mode="conv">
    <xsl:param name="maxocc"/>

    <xsl:apply-templates mode="conv">
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:group[@ref]" mode="conv">
    <xsl:variable name="ref" select="@ref"/>
    <xsl:variable name="ref_nons">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="$ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:comment>
    <xsl:value-of select="$ref"/>
    </xsl:comment>

    <xsl:apply-templates
      select="/xsd:schema/xsd:group[@name=$ref_nons]" mode="conv">
      <xsl:with-param name="maxocc" select="@maxOccurs"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence[@maxOccurs]" mode="conv">
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="conv">
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:sequence" mode="conv">
    <xsl:apply-templates mode="conv"/>
  </xsl:template>

  <xsl:template match="xsd:simpleContent" mode="conv">
    <xsl:apply-templates mode="conv"/>
  </xsl:template>

  <xsl:template match="xsd:extension" mode="conv">
    <xsl:variable name="type">
      <xsl:call-template name="make_type">
        <xsl:with-param name="type" select="@base"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="stem">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="@base"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:comment>
      <xsl:text> EXT </xsl:text>
      <xsl:value-of select="@base"/>
      <xsl:text> </xsl:text>
    </xsl:comment>

    <xsl:call-template name="make_slot">
      <xsl:with-param name="class" select="$stem"/>
      <xsl:with-param name="type" select="$type"/>
      <xsl:with-param name="slot" select="$stem"/>
    </xsl:call-template>

    <!-- more to come -->
    <xsl:apply-templates mode="conv"/>
  </xsl:template>

  <xsl:template match="xsd:choice[count(*) = 1]" mode="conv">
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:apply-templates mode="conv">
      <xsl:with-param name="maxocc" select="$maxocc"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="xsd:choice" mode="conv">
    <xsl:param name="maxocc" select="@maxOccurs"/>

    <xsl:choose>
      <xsl:when test="$maxocc = 'unbounded'">
        <!-- pseudo union, just fuck it -->
        <xsl:apply-templates mode="conv">
          <xsl:with-param name="maxocc" select="$maxocc"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:when test="@maxOccurs = 'unbounded'">
        <!-- pseudo union, just fuck it -->
        <xsl:apply-templates mode="conv">
          <xsl:with-param name="maxocc" select="@maxOccurs"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="union">
          <xsl:apply-templates mode="conv">
            <xsl:with-param name="maxocc" select="$maxocc"/>
          </xsl:apply-templates>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- special treatment for source/sequence -->
  <xsl:template match="xsd:choice[
                       xsd:element/@ref='mddl:sequence' and
                       xsd:element/@ref='mddl:source' and
                       count(xsd:element) = 2]" mode="conv">
    <xsl:comment> seq/src </xsl:comment>
    <xsl:call-template name="make_struct">
      <xsl:with-param name="type">
        <xsl:text>struct __sequence_s</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="slot">
        <xsl:text>sequence</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="mult">
        <xsl:text>1</xsl:text>
      </xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="make_struct">
      <xsl:with-param name="type">
        <xsl:text>struct __source_s</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="slot">
        <xsl:text>source</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="mult">
        <xsl:text>1</xsl:text>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- and now even more special treatment due to recurcurcurcursion -->
  <xsl:template match="xsd:choice[
                       xsd:element/@ref='mddl:sequence' and
                       xsd:element/@ref='mddl:source' and
                       count(xsd:element) = 2 and
                       (../../@name='Source' or ../../@name='Sequence')]"
    mode="conv">
  </xsl:template>

  <xsl:template match="xsd:attribute[@name]" mode="conv">
    <xsl:variable name="type">
      <xsl:choose>
        <xsl:when test="@type">
          <xsl:call-template name="make_stem">
            <xsl:with-param name="type">
              <xsl:value-of select="@type"/>
            </xsl:with-param>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>mdString</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:comment>
      <xsl:text>ATTR </xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:if test="@use">
        <xsl:text> USE:</xsl:text>
        <xsl:value-of select="@use"/>
      </xsl:if>
      <xsl:if test="@default">
        <xsl:text> DEFAULT:</xsl:text>
        <xsl:value-of select="@default"/>
      </xsl:if>
    </xsl:comment>

    <xsl:call-template name="make_slot">
      <xsl:with-param name="class" select="$type"/>
      <xsl:with-param name="type">
        <xsl:text>mddl_</xsl:text>
        <xsl:value-of select="$type"/>
        <xsl:text>_t</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="slot">
        <xsl:value-of select="@name"/>
      </xsl:with-param>
      <xsl:with-param name="use">
        <xsl:choose>
          <xsl:when test="@fixed">
            <xsl:text>mandatory</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="@use"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>
      <xsl:with-param name="default">
        <xsl:choose>
          <xsl:when test="@fixed">
            <xsl:value-of select="@fixed"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="@default"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="xsd:attribute[@ref]" mode="conv">
    <xsl:variable name="stem">
      <xsl:call-template name="make_stem">
        <xsl:with-param name="type" select="@ref"/>
      </xsl:call-template>
    </xsl:variable>

    <xsl:comment>
      <xsl:text> ATTR </xsl:text>
      <xsl:value-of select="@type"/>
      <xsl:if test="@use">
        <xsl:text> USE:</xsl:text>
        <xsl:value-of select="@use"/>
      </xsl:if>
      <xsl:if test="@default">
        <xsl:text> DEFAULT:</xsl:text>
        <xsl:value-of select="@default"/>
      </xsl:if>
    </xsl:comment>

    <xsl:call-template name="make_slot">
      <xsl:with-param name="class">
        <xsl:text>mdString</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="type">
        <xsl:call-template name="make_type">
          <xsl:with-param name="type">
            <xsl:text>mdString</xsl:text>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="slot">
        <xsl:value-of select="$stem"/>
      </xsl:with-param>
      <xsl:with-param name="use" select="@use"/>
      <xsl:with-param name="default" select="@default"/>
    </xsl:call-template>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="conv"/>
  <xsl:template match="xsd:*"/>
  <xsl:template match="xsd:*" mode="conv"/>

</xsl:stylesheet>
