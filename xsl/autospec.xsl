<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:UML="omg.org/UML1.3"
  version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="UML:Package">
    <xsl:text>PACK </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#0010;</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="UML:Package//UML:Class">
    <xsl:variable name="id" select="@xmi.id"/>

    <xsl:text>CLSS </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="$id"/>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates/>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates select="//UML:Association" mode="asso">
      <xsl:with-param name="id" select="$id"/>
    </xsl:apply-templates>
    <xsl:text>&#0010;</xsl:text>

    <xsl:apply-templates select="//UML:Generalization" mode="gnrl">
      <xsl:with-param name="id" select="$id"/>
    </xsl:apply-templates>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="UML:Attribute">
    <xsl:text>  ATTR </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:apply-templates/>
    <xsl:text>&#0010;</xsl:text>
  </xsl:template>

  <xsl:template match="UML:Attribute/UML:ModelElement.taggedValue">
    <xsl:text>&#0009;</xsl:text>
    <xsl:value-of select="UML:TaggedValue[@tag='type']/@value"/>
    <xsl:text>&#0009;[</xsl:text>
    <xsl:value-of select="UML:TaggedValue[@tag='lowerBound']/@value"/>
    <xsl:text>,</xsl:text>
    <xsl:value-of select="UML:TaggedValue[@tag='upperBound']/@value"/>
    <xsl:text>]</xsl:text>
  </xsl:template>

  <xsl:template match="UML:Association" mode="asso">
    <xsl:param name="id"/>
    <xsl:apply-templates mode="asso">
      <xsl:with-param name="id" select="$id"/>
    </xsl:apply-templates>
  </xsl:template>

  <xsl:template match="UML:Association.connection" mode="asso">
    <xsl:param name="id"/>

    <xsl:if test="UML:AssociationEnd[2]/@type=$id">
      <xsl:text>  ASSO </xsl:text>
      <xsl:apply-templates mode="asso"/>
      <xsl:text>&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="UML:AssociationEnd[1]" mode="asso">
    <xsl:text>&#0009;</xsl:text>
    <xsl:call-template name="xmiid2name">
      <xsl:with-param name="id" select="@type"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@multiplicity"/>
  </xsl:template>

  <xsl:template match="UML:Generalization" mode="gnrl">
    <xsl:param name="id"/>

    <xsl:if test="@supertype=$id">
      <xsl:text>  GNRL </xsl:text>
      <xsl:call-template name="xmiid2name">
        <xsl:with-param name="id" select="@subtype"/>
      </xsl:call-template>
      <xsl:text>&#0010;</xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template name="xmiid2name">
    <xsl:param name="id"/>
    <xsl:value-of select="//*[@xmi.id=$id]/@name"/>
  </xsl:template>

  <!-- catch all -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="asso"/>
  <xsl:template match="text()" mode="gnrl"/>

</xsl:stylesheet>
